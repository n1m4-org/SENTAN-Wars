#include "Utility/Scene/SceneManager.h"
#include "GameScene.h"
#include"Utility/Scene/SceneRegistry.h"
#include "Character/Enemy/EnemyManager.h"
#include <Frame.h>

REGISTER_SCENE("GAME", GameScene)

using namespace Hagine;
void GameScene::Initialize() {
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

	pDrawSystem_->Register("Test_PreDraw", DrawLayer::PreEffect, [this](const ViewProjection& vp) {
		pSpriteManager_->DrawAll();
		pObjectManager_->Draw(vp);
		});

	// 敵マネージャの初期化
	pEnemyManager_ = std::make_unique<EnemyManager>();
	pEnemyManager_->Init();

	// ウェーブディレクターの初期化
	pWaveDirector_ = std::make_unique<WaveDirector>();
	pWaveDirector_->SetEnemyManager(pEnemyManager_.get());
	pWaveDirector_->Initialize();

	// フォローカメラの初期化
	this->InitializeFollowCamera(player_->GetWorldTransform());

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
	BaseScene::Finalize();
}

void GameScene::Update() {
	/// ===================================================
	/// 更新処理
	/// ===================================================

	// クリアタイムの計測
	if (isTimerRunning_) {
		clearTimer_ += Frame::DeltaTime();
	}

	pFollowCamera_->Update();

	pEnemyManager_->Update();
	pWaveDirector_->Update();

	pHudManager_->Update();

	// カメラの更新
	UpdateCamera();

	// シーン切り替えの更新
	ChangeScene();
}

void GameScene::Draw() {
	/// ===================================================
	/// 描画処理
	/// ===================================================
}

void GameScene::DrawForOffScreen() {
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

void GameScene::AddObjectSetting() {
	/// ===================================================
	/// オブジェクト設定（デバッグ）
	/// ===================================================
}

void GameScene::AddParticleSetting() {
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

void GameScene::UpdateCamera() {
	/// ===================================================
	/// カメラ更新
	/// ===================================================
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	} else {
		vp_.UpdateMatrix();
	}
}

void GameScene::ChangeScene() {
	/// ===================================================
	/// シーン切り替え
	/// ===================================================
}

void GameScene::InitializeHudManager()
{
	pHudManager_ = std::make_unique<HudManager>();
	pHpHudView_ = pHudManager_->CreateView<HpHudView>();
	pWaveCountHudView_ = pHudManager_->CreateView<WaveCountHudView>();
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
