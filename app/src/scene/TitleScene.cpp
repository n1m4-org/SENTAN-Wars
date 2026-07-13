#include "TitleScene.h"
#include <scene/SceneRegistry.h>
#include <type/Vector4.h>
#include <utility/ViewportUnits.hpp>


REGISTER_SCENE("TITLE", TitleScene)

using namespace Hagine;

void TitleScene::Initialize()
{
    // シーン共通の初期化処理
    BaseScene::Initialize();
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{

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
    static constexpr const char* kTitleSpritePath = "common/space_485x120";
    static const Vector4 kWhite = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    {
        auto& sprite = GetSprite(SpriteName::Logo);
        Vector2 position = { 50_vw, 30_vh };
        sprite->Initialize(kLogo, position, kWhite, { 0.5f, 0.5f });
    }
    {
        auto& sprite = GetSprite(SpriteName::Prompt);
        Vector2 position = { 50_vw, 75_vh };
        sprite->Initialize(kTitleSpritePath, position, kWhite, { 0.5f, 0.5f });
    }
}
