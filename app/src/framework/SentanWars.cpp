#include "SentanWars.h"
#include <Frame.h>
#include <Object/Base/BaseObjectManager.h>

using namespace Hagine;

void SentanWars::Initialize() {
    this->Framework::Initialize();
    Framework::LoadResource();
    Framework::PlaySounds();
    Framework::RegisterShortcutKey();
    // -----ゲーム固有の処理-----
   
    // 最初のシーンを予約（シーンは REGISTER_SCENE で自己登録済み）
#ifdef _DEBUG
    pSceneManager_->NextSceneReservation("GAME");
#else
    pSceneManager_->NextSceneReservation("GAME");
#endif // _DEBUG
    // -----------------------

    /// デバッグエントリマネージャの初期化
    pDebugEntryManager_ = DebugEntryManager::GetInstance();
    pDebugEntryManager_->Initialize();
}

void SentanWars::Finalize() {
    // -----ゲーム固有の処理-----
    
    // -----------------------

    Framework::Finalize();
}

void SentanWars::Update() {
    Framework::Update();

    // -----ゲーム固有の処理-----
#ifdef _DEBUG
    if (imGuiManager_->GetEditorMode()) {
        pInput_->UpdateRay(*pSceneManager_->GetBaseScene()->GetViewProjection(), {imGuiManager_->GetScenePosForRay(), imGuiManager_->GetSceneSizeForRay()}, 10000.0f);
    } else {
        pInput_->UpdateRay(*pSceneManager_->GetBaseScene()->GetViewProjection(), {Vector2(0, 0), Vector2(winApp_->GetClientWidth(), winApp_->GetClientHeight())}, 10000.0f);
    }

    imGuiManager_->Begin();
    pImGuizmoManager_->BeginFrame();
    pImGuizmoManager_->SetViewProjection(pSceneManager_->GetBaseScene()->GetViewProjection());
    imGuiManager_->UpdateIni();
    imGuiManager_->SetCurrentScene(pSceneManager_->GetBaseScene());
    imGuiManager_->ShowMainMenu();
    if (imGuiManager_->GetIsShowMainUI()) {
        imGuiManager_->ShowDockSpace();
        imGuiManager_->ShowSceneWindow(offscreen_.get(), pSceneManager_->GetCurrentSceneName());
    }
    imGuiManager_->ShowMainUI(offscreen_.get());

    pDebugEntryManager_->ImGui();

    imGuiManager_->End();
#endif // _DEBUG
#ifndef _DEBUG
    pInput_->UpdateRay(*pSceneManager_->GetBaseScene()->GetViewProjection(), {Vector2(0, 0), Vector2(winApp_->GetClientWidth(), winApp_->GetClientHeight())}, 10000.0f);
#endif // _DEBUG

    // -----------------------
}

void SentanWars::Draw() {
    pDrawSystem_->Draw(*pSceneManager_->GetBaseScene()->GetViewProjection());

#ifdef _DEBUG
    imGuiManager_->Draw();
#endif // _DEBUG

    pDxCommon_->PostDraw();
#ifdef _DEBUG
    // メインの Present 後に、独立OSウィンドウへ切り離したImGuiウィンドウを描画する
    imGuiManager_->RenderMultiViewport();
#endif // _DEBUG
}