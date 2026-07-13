#include "SpriteFrameGroup.h"
#include <type/Vector2.h>
#include <algorithm>
#include <math/Mat3x3.h>

using namespace Hagine;

#undef min
#undef max

void SpriteFrameGroup::ComputeAndApply()
{
    Ni::Matrix3x3 worldMatrix =
        Ni::Matrix3x3::ScaleMatrix(property_.scale) *
        Ni::Matrix3x3::RotateMatrix({}) *
        Ni::Matrix3x3::TranslateMatrix(property_.position);

    for (auto& entry : entries_)
    {
        if (entry.sprite)
        {
            Vector2 spriteLeftTop = this->CalculateLeftTop(entry);
            // ワールド座標に変換
            Vector2 worldPosition = Ni::Transform(spriteLeftTop, worldMatrix);
            // スプライトの位置を更新
            entry.sprite->SetPosition(worldPosition);
        }
    }
}

Vector2 SpriteFrameGroup::ComputeGroupSize()
{
    Vector2 groupSize = { 0.0f, 0.0f };
    Vector2 minPosition = {};
    Vector2 maxPosition = {};
    for (const auto& entry : entries_)
    {
        if (entry.sprite)
        {
            Vector2 spriteSize = entry.sprite->GetSize();
            Vector2 spritePosition = this->CalculateLeftTop(entry);
            minPosition.x = std::min(minPosition.x, spritePosition.x);
            minPosition.y = std::min(minPosition.y, spritePosition.y);
            maxPosition.x = std::max(maxPosition.x, spritePosition.x + spriteSize.x);
            maxPosition.y = std::max(maxPosition.y, spritePosition.y + spriteSize.y);
        }
    }

    // グループのサイズを計算
    groupSize.x = maxPosition.x - minPosition.x;
    groupSize.y = maxPosition.y - minPosition.y;
    return groupSize;
}

Hagine::Vector2 SpriteFrameGroup::CalculateLeftTop(const Entry& entry)
{
    Vector2 spriteSize = entry.sprite->GetSize();
    Vector2 anchorOffset = { spriteSize.x * entry.anchorPoint.x, spriteSize.y * entry.anchorPoint.y };
    return entry.localPosition - anchorOffset;
}
