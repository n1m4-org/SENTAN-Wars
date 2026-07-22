#pragma once

#include <array>
#include <span>
#include <memory>
#include <vector>
#include "debug/GameParameter.h"
#include <font/FontLayout.h>
#include <Sprite.h>
#include <SpriteManager.h>
#include <Graphics/Texture/TextureManager.h>
#include <type/Vector4.h>
#include <type/Vector2.h>

class NumericView
{
public:
    /// エイリアス
    using TexHandleType = std::string;
    using SpriteMgr = Hagine::SpriteManager;
    using TexMgr = Hagine::TextureManager;
    using Sprite = Hagine::Sprite;
    using Vec2 = Hagine::Vector2;
    using Vec4 = Hagine::Vector4;
    
    void Initialize(
        std::span<TexHandleType> textureHandles, 
        const std::string& name
    );
    void Finalize();
    void Update();

    // デバッグUIの描画 (非ウィンドウ)
    void ImGui();

    // フォントサイズを取得
    float GetFontSize() const { return fontSizeY_; }

    // フォントサイズを設定 (Y値指定 / Xは比率維持して算出)
    void SetFontSize(float sizeY);

    // 現在の数字を設定
    void SetNumber(uint32_t number) { currentNumber_ = number; }

    // 数字の色を設定
    void SetColor(const Vec4& color);

    // フォントレイアウトのプロパティを取得
    FontLayout::Properties& GetFontLayoutProperties() { return fontLayout_.GetProperties(); }

    // 数字の色を取得
    Vec4 GetColor() const { return color_; }

    // 全体のサイズを取得 (注: Updateで更新)
    Vec2 GetSizeOverall() const { return fontLayout_.GetSizeOverall(); }

private:
    // スプライトを追加
    void AddSprite();

    // 現在の数字の桁数を取得
    uint32_t GetDigitCount() const;

    // 10進数の基数
    static constexpr uint32_t kRadix_ = 10u;

    // 数字のテクスチャハンドル
    std::array<TexHandleType, kRadix_> numberTextureHandles_ = {};

    // デバッグ用の名前
    std::string debugName_;

    // フォントレイアウト
    FontLayout fontLayout_;

    // 数字スプライトとグリフ情報
    std::vector<std::unique_ptr<Sprite>> numberSprites_;
    std::vector<GlyphInfo> glyphInfos_;

    // フォントサイズ（Y方向）
    float fontSizeY_ = 32.0f;

    // 数字の色
    Vec4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};

    // 現在の数字
    uint32_t currentNumber_ = 0;

    EnableDebug("NumericView");
};