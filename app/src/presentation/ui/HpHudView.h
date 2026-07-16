#pragma once
#include <SpriteManager.h>

class HpHudView
{
public:
    void Initialize();
    void Update(float hp, float hpMax);

private:
    // スプライトの初期化
    void InitializeSprites();

    // 「HP」という文字列のスプライト
    Hagine::SpriteData* pSpriteHp_ = nullptr;

    // HPバーのスプライト
    Hagine::SpriteData* pSpriteBar_ = nullptr;

    // HPバーの背景スプライト
    Hagine::SpriteData* pSpriteBarBg_ = nullptr;
};