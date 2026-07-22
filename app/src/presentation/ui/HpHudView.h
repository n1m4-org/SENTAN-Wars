#pragma once
#include "IHudView.h"
#include <utility/layout/FlexContainer.h>
#include "Bar2d.h"
#include <Sprite.h>
#include <memory>
#include <vector>
#include <debug/GameParameter.h>

class HpHudView : public IHudView
{
public:
    HpHudView();
    void Update() override;

    // HPの設定
    void SetHP(float hp) { currentHp_ = hp; }

    // 最大HPの設定
    void SetMaxHP(float maxHp) { maxHp_ = maxHp; }

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
    GameParameter(FlexBox, containerBox_, FlexBox());

    float currentHp_ = 100.0f;
    float maxHp_ = 100.0f;
};