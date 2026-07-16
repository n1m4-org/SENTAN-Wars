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

    pFlexContainer_->direction_ = FlexDirection::Row;
    pFlexContainer_->justifyContent_ = JustifyContent::Center;
    pFlexContainer_->alignItems_= AlignItems::Center;
    pFlexContainer_->gap_ = 16.0f;
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
    auto result = pFlexContainer_->Calculate(containerSize_, flexItems_);
    
    for (size_t i = 0; i < result.size(); ++i)
    {
        auto& sprite = sprites_[i + 1]; // Logoは除外するため+1
        sprite->SetPosition(result[i].position + containerPosition);
        sprite->SetSize(result[i].size);
    }

    pContainerArea_->SetPosition(containerPosition);
    pContainerArea_->SetSize(containerSize_);

    this->SpritePressedUpdate();
}

void TitleScene::Draw()
{
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

    pContainerArea_ = std::make_unique<Sprite>();
    pContainerArea_->Initialize("debug/white1x1.png", {}, { 0.0f, 0.0f, 0.0f, 0.5f }, {});

    pFlexContainer_ = std::make_unique<FlexContainer>();
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
        flexItems_.push_back({sprite->GetSize()});
    }
    {
        auto& sprite = GetSprite(SpriteName::StartKey);
        sprite = std::make_unique<Sprite>();
        sprite->Initialize(kStartKey, {}, kWhite, {});
        const auto& size = sprite->GetSize();
        sprite->SetSize(size * kStartKeyScale_);
        flexItems_.push_back({sprite->GetSize()});
    }

    SpriteManager* sm = SpriteManager::GetInstance();
    sm->RegisterExternal(pContainerArea_.get());
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

void TitleScene::FlexItemsUpdate()
{
    for (size_t i = 0; i < flexItems_.size(); ++i)
    {
        auto& sprite = sprites_[i + 1]; // Logoは除外するため+1
        flexItems_[i].preferredSize = sprite->GetSize();
    }
}
