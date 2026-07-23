#include "Utility/Scene/SceneManager.h"
#include "GameScene.h"
#include"Utility/Scene/SceneRegistry.h"
#include "Character/Enemy/EnemyManager.h"

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
	pWaveDirector_->Initialize();

	// フォローカメラの初期化
	this->InitializeFollowCamera(player_->GetWorldTransform());

	// 移動と体の向きをカメラ基準にする
	// 渡さないとワールド軸で動くため、カメラを回すと画面の向きと合わなくなる
	player_->SetReferenceCamera(&pFollowCamera_->GetViewProjection());

	// HUDの初期化
	this->InitializeHudManager();
}

void GameScene::Finalize() {
	/// ===================================================
	/// 終了処理
	/// ===================================================
	pEnemyManager_->Finalize();
	BaseScene::Finalize();
}

void GameScene::Update() {
	/// ===================================================
	/// 更新処理
	/// ===================================================

	pFollowCamera_->Update();

	pEnemyManager_->Update();

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
