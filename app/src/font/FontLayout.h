#pragma once
#include <type/Vector2.h>
#include <vector>
#include <span>

struct GlyphInfo
{
    Hagine::Vector2 size;
};

/// <summary>
/// フォントレイアウト計算クラス
/// 一つのテキストボックスのようなイメージ
/// </summary>
class FontLayout
{
public:
    using Vec2 = Hagine::Vector2;

    struct Result
    {
        Vec2 leftTop;
    };

    struct Properties
    {
        Vec2 leftTop = { 0.0f, 0.0f };
        Vec2 anchorPoint = {};
        float lineSpacing = 0.0f;
        float letterSpacing = 0.0f;
    };

    Properties& GetProperties() { return properties_; }
    std::vector<Result> Compute(std::span<GlyphInfo> glyphs);
    Vec2 GetSizeOverall() const { return sizeOverall_; }

private:
    Properties properties_;
    Vec2 sizeOverall_ = {};
};