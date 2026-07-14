#include "TitleScene.h"
#include <scene/SceneRegistry.h>
#include <type/Vector4.h>
#include <utility/ViewportUnits.hpp>
#include <SpriteManager.h>


REGISTER_SCENE("TITLE", TitleScene)

using namespace Hagine;

void TitleScene::Initialize()
{
    drawSystem_->Register("Test_PreDraw", DrawLayer::kPreEffect, [this](const ViewProjection& vp)
    {
        spriteManager_->DrawAll();
        objectManager_->Draw(vp);
    });

    // シーン共通の初期化処理
    BaseScene::Initialize();
    this->SpriteInitialize();
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
    pSpriteFrameGroup_->ComputeAndApply();
    this->SpritePressedUpdate();
}

void TitleScene::Draw()
{
    pSpriteFrameGroup_->DrawArea();
}

void TitleScene::DrawForOffScreen()
{

}

void TitleScene::AddSceneSetting()
{

}

void TitleScene::AddObjectSetting()
{

}

void TitleScene::AddParticleSetting()
{

}

void TitleScene::SpriteInitialize()
{
    static constexpr const char* kLogo = "common/logo_580x160.png";
    static constexpr const char* kStartKey = "common/space_485x120.png";
    static constexpr const char* kPrompt = "prompt/start_163x53.png";
    static const Vector4 kWhite = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    pSpriteFrameGroup_ = std::make_unique<SpriteFrameGroup>();
    pSpriteFrameGroup_->SetProperty({ 725.0f, 720.0f });
    {
        auto& sprite = GetSprite(SpriteName::Logo);
        sprite = std::make_unique<Sprite>();
        Vector2 position = { 50_vw, 25_vh };
        sprite->Initialize(kLogo, position, kWhite, { 0.5f, 0.5f });
    }
    {
        auto& sprite = GetSprite(SpriteName::Prompt);
        sprite = std::make_unique<Sprite>();
        sprite->Initialize(kPrompt, {}, kWhite, {});
        const auto& size = sprite->GetSize();
        sprite->SetSize(size * kPromptScale_);

        SpriteFrameGroup::Entry entry;
        entry.name = "Prompt";
        entry.sprite = sprite.get();
        entry.localPosition = { 245.0f, 0.0f };
        entry.anchorPoint = { 0.5f, 0.5f };
        pSpriteFrameGroup_->Add(entry);
    }
    {
        auto& sprite = GetSprite(SpriteName::StartKey);
        sprite = std::make_unique<Sprite>();
        sprite->Initialize(kStartKey, {}, kWhite, {});
        const auto& size = sprite->GetSize();
        sprite->SetSize(size * kStartKeyScale_);

        SpriteFrameGroup::Entry entry;
        entry.name = "StartKey";
        entry.sprite = sprite.get();
        entry.localPosition = { 0.0f, 0.0f };
        entry.anchorPoint = { 0.5f, 0.5f };
        pSpriteFrameGroup_->Add(entry);
    }

    SpriteManager* sm = SpriteManager::GetInstance();
    for (auto& sprite : sprites_)
    {
        sm->RegisterExternal(sprite.get());
    }
}

void TitleScene::SpritePressedUpdate()
{
    if (input_->PushKey(DIK_SPACE))
    {
        this->GetSprite(SpriteName::StartKey)->SetColor({ 0.7f, 0.7f, 0.7f });
    }
    else
    {
        this->GetSprite(SpriteName::StartKey)->SetColor({ 1.0f, 1.0f, 1.0f });
    }
}
