#pragma once
#include <vector>
#include <Sprite.h>
#include "debug/GameParameter.h"
#include <math/Color.h>

class SpriteFrameGroup
{
public:
    SpriteFrameGroup();

    struct FrameProperty
    {
        Hagine::Vector2 standard = {};
    };

    struct FrameData
    {
        Hagine::Sprite* sprite = nullptr;
        Hagine::Vector2 position = {};
    };

    struct Entry
    {
        std::string name;
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

    void DrawArea();

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

    std::unique_ptr<Hagine::Sprite> pPoint_ = nullptr;
    HSV debugAreaColor_ = { 1.0f, 0.65f, 0.85f };
    EnableDebug("SpriteFrameGroup");
};