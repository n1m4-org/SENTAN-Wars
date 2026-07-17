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

    debugCamera_ = std::make_unique<DebugCamera>();
    debugCamera_->Initialize(&vp_);

    // playerの初期化
	player_ = std::make_unique<Player>();
    player_->Init("Player");
	
    objectManager_->RegisterExternal(player_.get());

    drawSystem_->Register("Test_PreDraw", DrawLayer::kPreEffect, [this](const ViewProjection &vp) {
        spriteManager_->DrawAll();
        objectManager_->Draw(vp);
    });

    // 敵マネージャの初期化
	pEnemyManager_ = std::make_unique<EnemyManager>();
    pEnemyManager_->Init();

    // ウェーブディレクターの初期化
    pWaveDirector_ = std::make_unique<WaveDirector>();
    pWaveDirector_->Initialize();

    // フォローカメラの初期化
    this->InitializeFollowCamera(player_->GetWorldTransform());

    // HP HUDの初期化
    pHpHudView_ = std::make_unique<HpHudView>();
    pHpHudView_->Initialize();
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

    pHpHudView_->Update(70.0f, 100.0f);

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
    pFollowCamera_->Initialize(winApp_);
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
