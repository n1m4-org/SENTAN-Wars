#pragma once
#include <vector>
#include "FlexTypes.h"
#include <span>
#include <type/Vector2.h>
#include "debug/GameParameter.h"

class FlexContainer
{
public:
    FlexContainer();
    ~FlexContainer() = default;

    FlexDirection   direction_      = FlexDirection::Row;
    JustifyContent  justifyContent_ = JustifyContent::FlexStart;
    AlignItems      alignItems_     = AlignItems::Stretch;
    float           gap_            = 0.0f;

    std::vector<FlexResult> Calculate(const Hagine::Vector2& containerSize, std::span<const FlexItem> items) const;

private:
    float MainOf(const Hagine::Vector2& v) const;
    float CrossOf(const Hagine::Vector2& v) const;
    Hagine::Vector2 ToVec2(float main, float cross) const;
    void ApplyJustify(float freeSpace, std::span<const float> itemMainSizes, std::vector<float>& outPosition) const;
    CrossAlignResult ApplyCrossAlign(float itemCross, float containerCross, const FlexItem& item) const;

    EnableDebug("FlexContainer");
};