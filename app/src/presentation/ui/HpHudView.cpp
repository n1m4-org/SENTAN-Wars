#include "HpHudView.h"
#include <common/ResourcePath.h>


void HpHudView::Initialize()
{
    this->InitializeSprites();
}

void HpHudView::Update(float hp, float hpMax)
{

}

void HpHudView::InitializeSprites()
{
    constexpr const char* kHpSpriteName = "HPHUDVIEW::hp";
    constexpr const char* kBarSpriteName = "HPHUDVIEW::bar";
    constexpr const char* kBarBgSpriteName = "HPHUDVIEW::barBg";

    /// スプライトマネージャに登録
    auto sm = Hagine::SpriteManager::GetInstance();
    sm->RegisterSprite(kHpSpriteName, Path::Image::Hp);
    sm->RegisterSprite(kBarSpriteName, Path::Image::Bar);
    sm->RegisterSprite(kBarBgSpriteName, Path::Image::Bar);

    pSpriteHp_ = sm->GetSprite(kHpSpriteName);
    pSpriteBar_ = sm->GetSprite(kBarSpriteName);
    pSpriteBarBg_ = sm->GetSprite(kBarBgSpriteName);
}
