#include "Utility/Scene/SceneManager.h"
#include "GameScene.h"
#include"Utility/Scene/SceneRegistry.h"
#include <src/Character/Enemy/EnemyManager.h>

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

    pWaveDirector_ = std::make_unique<WaveDirector>();
    pWaveDirector_->Initialize();

    pFollowCamera_ = std::make_unique<FollowCamera>(vp_);
    pFollowCamera_->Initialize();
    pFollowCamera_->SetTarget(player_->GetWorldTransform());
}

void GameScene::Finalize() {
    /// ===================================================
    /// 終了処理
    /// ===================================================
    BaseScene::Finalize();
}

void GameScene::Update() {
    /// ===================================================
    /// 更新処理
    /// ===================================================

    pFollowCamera_->Update();

    // カメラの更新
    CameraUpdate();

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

void GameScene::CameraUpdate() {
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
