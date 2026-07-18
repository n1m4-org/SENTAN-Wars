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
    struct Result
    {
        Hagine::Vector2 leftTop;
    };

    struct Properties
    {
        Hagine::Vector2 leftTop = { 0.0f, 0.0f };
        Hagine::Vector2 anchorPoint = {};
        float lineSpacing = 0.0f;
        float letterSpacing = 0.0f;
    };

    Properties& GetProperties() { return properties_; }
    std::vector<Result> Compute(std::span<GlyphInfo> glyphs);

private:
    Properties properties_;
    float widthOverall = 0.0f;
};