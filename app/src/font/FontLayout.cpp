#include "FontLayout.h"
#include <algorithm>

std::vector<FontLayout::Result> FontLayout::Compute(std::span<GlyphInfo> glyphs)
{
    auto result = std::vector<Result>(glyphs.size());
    Vec2 penPosition = { 0.0f, 0.0f };

    /// 各グリフの左上座標を計算
    for (size_t i = 0; i < glyphs.size(); ++i)
    {
        result[i].leftTop = properties_.leftTop + penPosition;
        result[i].leftTop.y -= glyphs[i].size.y * properties_.anchorPoint.y;
        penPosition.x += glyphs[i].size.x + properties_.letterSpacing;
    }

    /// サイズ計算
    float width = penPosition.x - properties_.letterSpacing;
    float height = 0.0f;
    for (auto& g : glyphs)
    {
        height = std::max(height, g.size.y);
    }
    sizeOverall_ = { width, height };

    /// アンカーポイントを考慮して左上座標を調整
    for (auto& res : result)
    {
        res.leftTop.x -= width * properties_.anchorPoint.x;
    }

    return result;
}
