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

    void SetRemainingCount(uint32_t count)
    {
        pNvCount_->SetNumber(count);
    }

private:
    void InitializeFlexContainers();
    void InitializeNumericView();
    void InitializeSprites();
    void ApplyFlexLayout();

    EnableDebug("RemainingEnemyCountHudView");

    FlexContainer containerOverall_;
    GameParameter(FlexBox, flexBoxOverall_, FlexBox());
    FlexContainer containerCount_;
    GameParameter(FlexBox, flexBoxCount_, FlexBox());

    std::unique_ptr<Hagine::Sprite> pSpriteRemain_ = nullptr;
    std::unique_ptr<Hagine::Sprite> pSpriteUnit_ = nullptr;
    std::unique_ptr<NumericView> pNvCount_ = nullptr;

    std::vector<FlexItem> flexItemsOverall_;
    std::vector<FlexItem> flexItemsCount_;
};