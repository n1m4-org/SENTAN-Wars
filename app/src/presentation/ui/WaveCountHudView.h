#pragma once
#include "IHudView.h"
#include <memory>
#include <font/NumericView.h>
#include <Sprite.h>
#include <utility/layout/FlexContainer.h>
#include <utility/layout/FlexTypes.h>
#include <vector>
#include <type/Vector2.h>


class WaveCountHudView : public IHudView
{
public:
    WaveCountHudView();
    void Update() override;

private:
    void InitializeFlexContainer();
    void InitializeNumericView();
    void UpdateFlexItems();

    FlexBox containerBox_ = {};

    std::unique_ptr<FlexContainer> pFlexContainer_ = nullptr;
    std::unique_ptr<NumericView> pNumericView_ = nullptr;
    std::unique_ptr<Hagine::Sprite> pWaveSprite_ = nullptr;
    std::vector<FlexItem> flexItems_ = {};
};