#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <Sprite.h>
#include <type/Vector2.h>
#include <type/Vector4.h>

/// <summary>
/// クリアタイムを "MM:SS" 形式で描画するビュー。
/// 数字スプライトとコロンスプライトを左詰めで並べる。
/// フォントサイズ（高さ）で拡大縮小の演出に対応する。
/// </summary>
class ClearTimeView
{
public:
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// 終了処理（スプライトの登録解除）
    /// </summary>
    void Finalize();

    /// <summary>
    /// 更新処理（レイアウトの再計算）
    /// </summary>
    void Update();

    /// <summary>
    /// 表示する秒数を設定（MM:SS に変換して描画）
    /// </summary>
    /// <param name="seconds">秒数</param>
    void SetSeconds(float seconds);

    /// <summary>
    /// 左上原点を設定
    /// </summary>
    /// <param name="origin">左上原点</param>
    void SetOrigin(const Hagine::Vector2& origin) { origin_ = origin; }

    /// <summary>
    /// フォントサイズ（数字の高さ）を設定
    /// </summary>
    /// <param name="fontSize">フォントサイズ</param>
    void SetFontSize(float fontSize) { fontSize_ = fontSize; }

    /// <summary>
    /// 文字色を設定
    /// </summary>
    /// <param name="color">色</param>
    void SetColor(const Hagine::Vector4& color) { color_ = color; }

    /// <summary>
    /// 現在のフォントサイズでの全体サイズを取得（Update で更新）
    /// </summary>
    /// <returns>Vector2: 全体サイズ</returns>
    Hagine::Vector2 GetOverallSize() const { return overallSize_; }

private:
    /// <summary>
    /// 描画するスロット（MM:SS の 5 文字）
    /// </summary>
    enum class Slot { MinTens, MinOnes, Colon, SecTens, SecOnes, kSize };

    static constexpr size_t kSlotCount_ = static_cast<size_t>(Slot::kSize);
    static constexpr uint32_t kColonGlyph_ = 10u; // 0-9 が数字、10 がコロン

    /// <summary>
    /// グリフのテクスチャパスを取得
    /// </summary>
    /// <param name="glyph">グリフ番号（0-9: 数字, 10: コロン）</param>
    /// <returns>テクスチャパス</returns>
    const char* GetGlyphPath(uint32_t glyph) const;

    /// <summary>
    /// グリフのアスペクト比（幅 / 高さ）を取得
    /// </summary>
    /// <param name="glyph">グリフ番号</param>
    /// <returns>アスペクト比</returns>
    float GetGlyphAspect(uint32_t glyph) const;

    std::array<std::unique_ptr<Hagine::Sprite>, kSlotCount_> sprites_ = {};
    std::array<uint32_t, kSlotCount_> glyphs_ = { 0u, 0u, kColonGlyph_, 0u, 0u };  // 初期 "00:00"
    std::array<uint32_t, kSlotCount_> appliedGlyphs_ = {};                          // 反映済みグリフ

    Hagine::Vector2 origin_ = {};                       // 左上原点
    Hagine::Vector2 overallSize_ = {};                  // 全体サイズ
    Hagine::Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
    float fontSize_ = 60.0f;                            // 数字の高さ
    float letterSpacing_ = 6.0f;                        // 文字間隔
};
