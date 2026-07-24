#include "Utility/Scene/SceneManager.h"
#include "GameScene.h"
#include "Utility/Scene/SceneRegistry.h"
#include "Character/Enemy/EnemyManager.h"
#include <Frame.h>
#include <logic/EquippedSentanCache.h>
#include <base/DirectXCommon.h>
#include <Debug/log/Logger.h>
#include <cstdio>

REGISTER_SCENE("GAME", GameScene)

using namespace Hagine;
void GameScene::Initialize()
{
    /// ===================================================
    /// 初期化
    /// ===================================================
    BaseScene::Initialize();
    vp_.Initialize();
    pLightGroup_->LoadLightData("GameLight");
    pObjectManager_->LoadAll("GameScene");

    debugCamera_ = std::make_unique<DebugCamera>();
    debugCamera_->Initialize(&vp_);

    // playerの初期化
    player_ = std::make_unique<Player>();
    player_->Init("Player");

    pObjectManager_->RegisterExternal(player_.get());

    pDrawSystem_->Register("Test_PreDraw", DrawLayer::PreEffect, [this](const ViewProjection& vp)
    {
        pSpriteManager_->DrawAll();
        pObjectManager_->Draw(vp);
    });

	// 敵マネージャの初期化
	pEnemyManager_ = std::make_unique<EnemyManager>();
	pEnemyManager_->Init();
	pEnemyManager_->SetTarget(&player_->GetWorldTransform()->translation_);

	// ウェーブディレクターの初期化
	pWaveDirector_ = std::make_unique<WaveDirector>();
	pWaveDirector_->SetEnemyManager(pEnemyManager_.get());
	// 準備フェーズで選んだSENTANの装備先（最初のウェーブにも渡るようInitializeより前に）
	pWaveDirector_->SetPlayer(player_.get());
	pWaveDirector_->Initialize();

    // フォローカメラの初期化
    this->InitializeFollowCamera(player_->GetWorldTransform());

	// 移動と体の向きをカメラ基準にする
	// 渡さないとワールド軸で動くため、カメラを回すと画面の向きと合わなくなる
	player_->SetReferenceCamera(&pFollowCamera_->GetViewProjection());

	// HUDの初期化
	this->InitializeHudManager();

	// クリアタイムの計測を開始
	clearTimer_ = 0.0f;
	isTimerRunning_ = true;
}

void GameScene::Finalize() {
	/// ===================================================
	/// 終了処理
	/// ===================================================
	this->StopClearTime();
	pEnemyManager_->Finalize();
    EquippedSentanCache::GetInstance()->Reset();
	BaseScene::Finalize();
}

void GameScene::Update()
{
    /// ===================================================
    /// 更新処理
    /// ===================================================

	// クリアタイムの計測
	if (isTimerRunning_) {
		clearTimer_ += Frame::DeltaTime();
	}

	// GPUデバイスが落ちていないか見張る
	// 描画に使われているリソースを解放してしまうとデバイスが失われ、
	// 以後のリソース確保が全部失敗する（落ちるのは次に何かを作ったところになるため）
	this->CheckDeviceLost();

	pFollowCamera_->Update();

	pEnemyManager_->Update();
	pWaveDirector_->Update();

    pHudManager_->Update();

    // カメラの更新
    UpdateCamera();

    // シーン切り替えの更新
    ChangeScene();

    // HUDの更新
    this->UpdateHud();
}

void GameScene::Draw()
{
    /// ===================================================
    /// 描画処理
    /// ===================================================
}

void GameScene::DrawForOffScreen()
{
    /// ===================================================
    /// オフスクリーン描画処理
    /// ===================================================
}

void GameScene::AddSceneSetting() {
	/// ===================================================
	/// シーン設定（デバッグ）
	/// ===================================================
	debugCamera_->imgui();

#ifdef _DEBUG
	// クリアタイムの計測確認用（本番のクリア判定が入るまでの動作確認用ボタン）
	ImGui::Text("Play Time: %.2f s", clearTimer_);
	ImGui::Text("Timer Running: %s", isTimerRunning_ ? "true" : "false");
	if (ImGui::Button("Clear -> CLEAR Scene")) {
		this->StopClearTime();
		pSceneManager_->NextSceneReservation("CLEAR");
	}
#endif // _DEBUG
}

void GameScene::AddObjectSetting()
{
    /// ===================================================
    /// オブジェクト設定（デバッグ）
    /// ===================================================
}

void GameScene::AddParticleSetting()
{
    /// ===================================================
    /// パーティクル設定（デバッグ）
    /// ===================================================
}

void GameScene::InitializeFollowCamera(const Hagine::WorldTransform* pTarget)
{
    pFollowCamera_ = std::make_unique<FollowCamera>(vp_);
    pFollowCamera_->Initialize(pWinApp_);
    pFollowCamera_->SetTarget(pTarget);
}

void GameScene::UpdateCamera()
{
    /// ===================================================
    /// カメラ更新
    /// ===================================================
    if (debugCamera_->GetActive())
    {
        debugCamera_->Update();
    }
    else
    {
        vp_.UpdateMatrix();
    }
}

void GameScene::ChangeScene()
{
    /// ===================================================
    /// シーン切り替え
    /// ===================================================
    if (pWaveDirector_->IsGameCleared())
    {
        this->StopClearTime();
        pSceneManager_->NextSceneReservation("CLEAR");
    }
}

void GameScene::InitializeHudManager()
{
    pHudManager_ = std::make_unique<HudManager>();
    pHpHudView_ = pHudManager_->CreateView<HpHudView>();
    pWaveCountHudView_ = pHudManager_->CreateView<WaveCountHudView>();
    pRemainEnemyCountHudView_ = pHudManager_->CreateView<RemainingEnemyCountHudView>();
}

void GameScene::UpdateHud()
{
    pWaveCountHudView_->SetWaveCount(pWaveDirector_->GetCurrentWaveIndex() + 1);
    pRemainEnemyCountHudView_->SetRemainingCount(pEnemyManager_->GetTotalRemainingEnemyCount());

    // プレイヤーの実HPをバーに反映する（被弾するとここ経由でバーが減る）
    pHpHudView_->SetMaxHP(player_->GetMaxHp());
    pHpHudView_->SetHP(player_->GetHp());
}

void GameScene::CheckDeviceLost() {
	/// ===================================================
	/// GPUデバイスが失われていないかの確認
	/// ===================================================
	if (isDeviceLostReported_) {
		return;
	}

	ID3D12Device* pDevice = DirectXCommon::GetInstance()->GetDevice().Get();
	if (!pDevice) {
		return;
	}

	const HRESULT reason = pDevice->GetDeviceRemovedReason();
	if (SUCCEEDED(reason)) {
		return;
	}

	isDeviceLostReported_ = true;

	// 失われた瞬間のウェーブ番号まで残しておくと、どの処理が壊したか絞り込める
	char message[256]{};
	std::snprintf(message, sizeof(message), "GPU device lost. reason=0x%08X wave=%u",
		static_cast<unsigned int>(reason), pWaveDirector_ ? pWaveDirector_->GetCurrentWaveIndex() + 1 : 0);
	Logger::Info(message);
}

void GameScene::StopClearTime() {
	/// ===================================================
	/// クリアタイムの計測を停止し、SceneManager に記録する
	/// ===================================================
	if (!isTimerRunning_) {
		// すでに停止済みなら二重に記録しない
		return;
	}
	isTimerRunning_ = false;
	pSceneManager_->SetClearTime(clearTimer_);
}
