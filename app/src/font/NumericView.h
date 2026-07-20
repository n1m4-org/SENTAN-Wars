#pragma once
#include <d3d12.h>

#include <array>
#include <span>
#include <vector>
#include <Sprite.h>
#include <font/FontLayout.h>
#include <type/Vector4.h>
#include <memory>
#include "debug/GameParameter.h"

class NumericView
{
public:
    using TextureHandleType = std::string;
    
    void Initialize(
        std::span<TextureHandleType> textureHandles, 
        const std::string& name
    );
    void Finalize();

    void Update();
    void ImGui();

    float GetFontSize() const { return fontSizeY_; }
    void SetFontSize(float sizeY);
    void SetNumber(uint32_t number) { currentNumber_ = number; }
    void SetColor(const Hagine::Vector4& color);
    FontLayout::Properties& GetFontLayoutProperties() { return fontLayout_.GetProperties(); }
    Hagine::Vector4 GetColor() const { return color_; }

private:
    void AddSprite();
    uint32_t GetDigitCount() const;
    static constexpr uint32_t kRadix_ = 10u;

    std::array<TextureHandleType, kRadix_> numberTextureHandles_ = {};

    // デバッグ用の名前
    std::string debugName_;
    // フォントレイアウト
    FontLayout fontLayout_;

    std::vector<std::unique_ptr<Hagine::Sprite>> numberSprites_;
    std::vector<GlyphInfo> glyphInfos_;

    float fontSizeY_ = 32.0f;

    Hagine::Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};

    uint32_t currentNumber_ = 0;

    EnableDebug("NumericView");
};