#include "ClearScene.h"
#include <algorithm>
#include <common/ResourcePath.h>
#include <Easing.h>
#include <Frame.h>
#include <scene/SceneRegistry.h>
#include <scene/SceneManager.h>
#include <SpriteManager.h>
#include <utility/SpriteUnregisterer.h>
#include <WinApp.h>

REGISTER_SCENE("CLEAR", ClearScene)

using namespace Hagine;

void ClearScene::Initialize()
{

    // シーン共通の初期化処理
    BaseScene::Initialize();

	//pLightGroup_->LoadLightData("GameLight");
    //pObjectManager_->LoadAll("GameScene");

    // スプライトの描画を登録する
    pDrawSystem_->Register("Clear_PreDraw", DrawLayer::PreEffect, [this](const ViewProjection& vp)
        {
            pSpriteManager_->DrawAll();
            pObjectManager_->Draw(vp);
        });

    // クリアタイムをゲーム側（SceneManager 経由）から取得する。
    // 0（未計測・CLEAR を直接起動した場合）はデバッグ用のデフォルト値を維持する。
    const float recordedClearTime = pSceneManager_->GetClearTime();
    if (recordedClearTime > 0.0f)
    {
        targetClearSeconds_ = recordedClearTime;
    }

    // レイアウトとスプライトの初期化
    this->InitializeLayout();
    this->InitializeSprites();

    // 初期状態（枠は展開前、クリアタイムは非表示）を反映する
    phase_ = Phase::FrameExpand;
    this->ApplyReveal(0.0f);
    this->ApplyCountUp(0.0f);
    clearTimeView_.Update();
}

void ClearScene::Finalize()
{
    frame_.Finalize();
    clearTimeView_.Finalize();

    Sprite* labels[] = { pClearTimeLabel_.get() };
    utl::sprite::Unregister(labels);
}

void ClearScene::Update()
{
    const float deltaTime = Frame::DeltaTime();

    // SPACE で演出をスキップする
    if (phase_ != Phase::Done && pInput_->TriggerKey(DIK_SPACE))
    {
        this->Skip();
    }

    switch (phase_)
    {
    case Phase::FrameExpand:
        // 枠を横 → 縦 の順に展開する
        frame_.Update(deltaTime);
        this->ApplyReveal(0.0f);
        this->ApplyCountUp(0.0f);
        if (frame_.IsFinished())
        {
            phase_ = Phase::Reveal;
        }
        break;

    case Phase::Reveal:
    {
        // クリアタイムを出現（小 → 大）させつつカウントアップする
        revealTimer_ = (std::min)(revealTimer_ + deltaTime, kRevealDuration_);
        countTimer_ = (std::min)(countTimer_ + deltaTime, kCountDuration_);

        const float scale = ApplyEasing<float>(EasingType::OutBack, 0.0f, 1.0f, revealTimer_, kRevealDuration_);
        this->ApplyReveal(scale);

        const float seconds = ApplyEasing<float>(EasingType::OutCubic, 0.0f, targetClearSeconds_, countTimer_, kCountDuration_);
        this->ApplyCountUp(seconds);

        if (revealTimer_ >= kRevealDuration_ && countTimer_ >= kCountDuration_)
        {
            phase_ = Phase::Done;
        }
        break;
    }

    case Phase::Done:
        // 最終状態を維持する
        this->ApplyReveal(1.0f);
        this->ApplyCountUp(targetClearSeconds_);
        break;
    }

    clearTimeView_.Update();
}

void ClearScene::AddSceneSetting() {
  //  debugCamera_->imgui();
}

void ClearScene::InitializeLayout()
{
    // 枠は画面中央に配置する
    const float screenWidth = static_cast<float>(WinApp::GetVirtualWidth());
    const float screenHeight = static_cast<float>(WinApp::GetVirtualHeight());

    frameSize_ = { 1120.0f, 680.0f };
    frameCenter_ = { screenWidth * 0.5f, screenHeight * 0.5f };

    const Vector2 topLeft = frameCenter_ - frameSize_ * 0.5f;

    // クリアタイムは枠の左上に配置する
    constexpr float kPadding = 56.0f;
    constexpr float kGap = 28.0f;

    leftX_ = topLeft.x + kPadding;
    labelCenterY_ = topLeft.y + kPadding + kLabelHeight_ * 0.5f;
    timeCenterY_ = labelCenterY_ + kLabelHeight_ * 0.5f + kGap + timeFullFontSize_ * 0.5f;
}

void ClearScene::InitializeSprites()
{
    // 枠（横 → 縦 に展開する長方形）
    ResultFrame::Config frameConfig;
    frameConfig.center = frameCenter_;
    frameConfig.size = frameSize_;
    frameConfig.borderThickness = 4.0f;
    frameConfig.lineThickness = 6.0f;
    frameConfig.horizontalDuration = 0.35f;
    frameConfig.verticalDuration = 0.35f;
    frameConfig.borderColor = { 0.55f, 0.55f, 0.55f, 1.0f };
    frameConfig.innerColor = { 0.0f, 0.0f, 0.0f, 0.55f };
    frame_.Initialize(frameConfig);

    // 「CLEAR TIME」のラベル（左中央アンカーでスケール演出する）
    pClearTimeLabel_ = std::make_unique<Sprite>();
    pClearTimeLabel_->Initialize(Path::Image::ClearTime, { leftX_, labelCenterY_ }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.5f });

    const Vector2 naturalSize = pClearTimeLabel_->GetSize();
    labelFullSize_ = { naturalSize.x * (kLabelHeight_ / naturalSize.y), kLabelHeight_ };

    SpriteManager::GetInstance()->RegisterExternal(pClearTimeLabel_.get());

    // クリアタイムの数字
    clearTimeView_.Initialize();
    clearTimeView_.SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
}

void ClearScene::Skip()
{
    frame_.Skip();
    revealTimer_ = kRevealDuration_;
    countTimer_ = kCountDuration_;
    phase_ = Phase::Done;
}

void ClearScene::ApplyReveal(float scale)
{
    // ラベル：左中央を固定したままスケールさせる
    pClearTimeLabel_->SetPosition({ leftX_, labelCenterY_ });
    pClearTimeLabel_->SetSize(labelFullSize_ * scale);

    // クリアタイム：左端と縦中心を固定したままフォントサイズをスケールさせる
    const float fontSize = timeFullFontSize_ * scale;
    clearTimeView_.SetFontSize(fontSize);
    clearTimeView_.SetOrigin({ leftX_, timeCenterY_ - fontSize * 0.5f });
}

void ClearScene::ApplyCountUp(float seconds)
{
    clearTimeView_.SetSeconds(seconds);
}
