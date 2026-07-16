#include "FlexContainer.h"
#include <algorithm>
#include <numeric>



std::vector<FlexResult> FlexContainer::Calculate(const Hagine::Vector2& containerSize, std::span<const FlexItem> items) const
{
    std::vector<FlexResult> results;

    // 1. 各アイテムのメイン軸サイズとクロス軸サイズを計算
    std::vector<float> itemMainSizes, itemCrossSizes;

    for (const auto& item : items)
    {
        float mainSize = MainOf(item.preferredSize);
        float crossSize = CrossOf(item.preferredSize);
        /// サイズの制約を適用
        mainSize = std::clamp(mainSize, MainOf(item.minSize), MainOf(item.maxSize));
        crossSize = std::clamp(crossSize, CrossOf(item.minSize), CrossOf(item.maxSize));
        itemMainSizes.push_back(mainSize);
        itemCrossSizes.push_back(crossSize);
    }

    // 2. アイテムのメイン軸サイズの合計を計算
    float itemMainSums_ = std::accumulate(itemMainSizes.begin(), itemMainSizes.end(), 0.0f);

    // 3. コンテナのメイン軸サイズからアイテムの合計サイズとギャップを引いて、余白を計算
    float freeSpace = MainOf(containerSize) - itemMainSums_ - gap_ * (items.size() - 1);

    // 4. Justify-Content
    std::vector<float> itemPositions;
    this->ApplyJustify(freeSpace, itemMainSizes, itemPositions);

    // 5. Align-Itemsを適用
    for (size_t i = 0; i < itemMainSizes.size(); ++i)
    {
        float itemCrossSize = itemCrossSizes[i];
        float containerCrossSize = CrossOf(containerSize);
        auto crossAlignResult = ApplyCrossAlign(itemCrossSize, containerCrossSize, items[i]);
        results[i].position = ToVec2(itemPositions[i], crossAlignResult.position);
        results[i].size = ToVec2(itemMainSizes[i], crossAlignResult.size);
    }

    return results;
}

float FlexContainer::MainOf(const Hagine::Vector2& v) const
{
    if (direction_ == FlexDirection::Row || direction_ == FlexDirection::RowReverse)
        return v.x;
    else
        return v.y;
}

float FlexContainer::CrossOf(const Hagine::Vector2& v) const
{
    if (direction_ == FlexDirection::Row || direction_ == FlexDirection::RowReverse)
        return v.y;
    else
        return v.x;
}

Hagine::Vector2 FlexContainer::ToVec2(float main, float cross) const
{
    return Hagine::Vector2(main, cross);
}

void FlexContainer::ApplyJustify(float freeSpace, std::span<const float> itemMainSizes, std::vector<float>& outPosition) const
{
    // 要素が空の場合は何もしない
    if (itemMainSizes.empty()) return;

    // 配列のサイズを確保
    outPosition.resize(itemMainSizes.size());

    // ref: https://developer.mozilla.org/ja/docs/Learn_web_development/Core/CSS_layout/Flexbox
    //      https://developer.mozilla.org/ja/docs/Web/CSS/Reference/Properties/justify-content

    if (justifyContent_ == JustifyContent::FlexStart)
    {
        float cursor = 0.0f;
        for (auto& itemSize : itemMainSizes)
        {
            outPosition.push_back(cursor);
            cursor += itemSize + gap_;
        }
    }
    else if (justifyContent_ == JustifyContent::FlexEnd)
    {
        float cursor = freeSpace;
        for (auto& itemSize : itemMainSizes)
        {
            outPosition.push_back(cursor);
            cursor += itemSize + gap_;
        }
    }
    else if (justifyContent_ == JustifyContent::Center)
    {
        float cursor = freeSpace / 2.0f;
        for (auto& itemSize : itemMainSizes)
        {
            outPosition.push_back(cursor);
            cursor += itemSize + gap_;
        }
    }
    else if (justifyContent_ == JustifyContent::SpaceBetween)
    {
        float spaceBetween = freeSpace / (itemMainSizes.size() - 1);
        float cursor = 0.0f;
        for (auto& itemSize : itemMainSizes)
        {
            outPosition.push_back(cursor);
            cursor += itemSize + gap_ + spaceBetween;
        }
    }
    else if (justifyContent_ == JustifyContent::SpaceAround)
    {
        float spaceAround = freeSpace / itemMainSizes.size();
        float cursor = spaceAround / 2.0f;
        for (auto& itemSize : itemMainSizes)
        {
            outPosition.push_back(cursor);
            cursor += itemSize + gap_ + spaceAround;
        }
    }
    else if (justifyContent_ == JustifyContent::SpaceEvenly)
    {
        float spaceEvenly = freeSpace / (itemMainSizes.size() + 1);
        float cursor = spaceEvenly;
        for (auto& itemSize : itemMainSizes)
        {
            outPosition.push_back(cursor);
            cursor += itemSize + gap_ + spaceEvenly;
        }
    }
}

CrossAlignResult FlexContainer::ApplyCrossAlign(float itemCross, float containerCross, const FlexItem& item) const
{
    if (alignItems_ == AlignItems::Stretch)
    {
        return { 0.0f, containerCross };
    }
    else if (alignItems_ == AlignItems::FlexStart)
    {
        return { 0.0f, itemCross };
    }
    else if (alignItems_ == AlignItems::FlexEnd)
    {
        return { containerCross - itemCross, itemCross };
    }
    else if (alignItems_ == AlignItems::Center)
    {
        return { (containerCross - itemCross) / 2.0f, itemCross };
    }
}
