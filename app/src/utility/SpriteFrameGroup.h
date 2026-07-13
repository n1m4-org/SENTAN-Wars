#pragma once
#include <vector>
#include <Sprite.h>
#include "debug/GameParameter.h"

class SpriteFrameGroup
{
public:
    struct FrameProperty
    {
        Hagine::Vector2 anchorPoint = {};
        Hagine::Vector2 position = {};
        Hagine::Vector2 scale = { 1.0f, 1.0f };
    };

    struct FrameData
    {
        Hagine::Sprite* sprite = nullptr;
        Hagine::Vector2 position = {};
    };

    struct Entry
    {
        Hagine::Sprite* sprite = nullptr;
        Hagine::Vector2 localPosition = {};
        Hagine::Vector2 anchorPoint = {};
    };

    void SetProperty(const FrameProperty& property)
    {
        property_ = property;
    }

    void Add(const Entry& entry)
    {
        entries_.push_back(entry);
    }

    /// <summary>
    /// グループのサイズを計算し、各スプライトの位置を調整する
    /// </summary>
    void ComputeAndApply();

private:
    Hagine::Vector2 ComputeGroupSize();
    Hagine::Vector2 CalculateLeftTop(const Entry& entry);

    FrameProperty property_;
    std::vector<Entry> entries_;

    /// +-----------+
    /// |   Debug   |
    /// +-----------+

    EnableDebug("SpriteFrameGroup");
};