#pragma once
#include "IHudView.h"
#include <utility/layout/FlexContainer.h>
#include <Sprite.h>
#include <font/NumericView.h>


class RemainingEnemyCountHudView : public IHudView
{
public:
    RemainingEnemyCountHudView();
    ~RemainingEnemyCountHudView() = default;

    void Update() override;

private:
    FlexContainer containerRemain_;
    FlexContainer containerCount_;

    std::unique_ptr<Hagine::Sprite> pSpriteRemain_ = nullptr;
    std::unique_ptr<Hagine::Sprite> pSpriteUnit_ = nullptr;
    std::unique_ptr<NumericView> pNvCount = nullptr;
};