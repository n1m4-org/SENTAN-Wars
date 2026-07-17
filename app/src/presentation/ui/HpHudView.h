#pragma once
#include <SpriteManager.h>
#include <utility/layout/FlexContainer.h>
#include "Bar2d.h"
#include <Sprite.h>
#include <memory>
#include <vector>

class HpHudView
{
public:
    void Initialize();
    void Update(float hp, float hpMax);

private:
    // スプライトの初期化
    void InitializeSprites();

    // フレックスコンテナの初期化
    void InitializeFlexContainer();

    // FlexBoxの計算と適用
    void ApplyFlexLayout();

    void RegisterCustomGui();

    // 「HP」という文字列のスプライト
    std::unique_ptr<Hagine::Sprite> pSpriteHp_ = nullptr;

    // HPバー
    Bar2d bar_;
    
    // フレックスコンテナ
    std::unique_ptr<FlexContainer> pFlexContainer_ = nullptr;

    // フレックスアイテム
    std::vector<FlexItem> flexItems_;

    EnableDebug("HpHudView");
    GameParameter(Hagine::Vector2, containerPosition_, Hagine::Vector2(0.0f, 860.0f));
    GameParameter(Hagine::Vector2, containerSize_, Hagine::Vector2(500.0f, 100.0f));

    float currentHp_ = 100.0f;
};