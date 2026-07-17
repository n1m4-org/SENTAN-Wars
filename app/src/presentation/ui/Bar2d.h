#pragma once
#include <memory>
#include <Sprite.h>
#include <type/Vector2.h>
#include <math/Color.h>
#include <optional>
#include <functional>

class Bar2d
{
public:
    struct Config
    {
        RGBA colorMain = 0xa82227ff;
        RGBA colorBg = 0x313131ff;
        std::optional<RGBA> warnColor = std::nullopt;
        float warnThreshold = 0.2f;
        float interpolationRatio = 1.0f; // 補間の速度（0.0f～1.0f）
    };

    void Initialize(const Config& cfg);
    void Update(float value, float valueMax);

    Hagine::Vector2 GetPosition() const { return leftTop_; }
    void SetPosition(const Hagine::Vector2& position) { leftTop_ = position; }

    Hagine::Vector2 GetSize() const;

    void SetMainColor(RGBA color);
    RGBA GetMainColor() const;

    void SetBackgroundColor(RGBA color);
    RGBA GetBackgroundColor() const;


private:
    // スプライトの初期化
    void InitializeSprites(RGBA colorMain, RGBA colorBg);

    // HPバーのスプライト
    std::unique_ptr<Hagine::Sprite> pSpriteBar_ = nullptr;

    // HPバーの背景スプライト
    std::unique_ptr<Hagine::Sprite> pSpriteBarBg_ = nullptr;

    // 座標
    Hagine::Vector2 leftTop_ = { 0.0f, 0.0f };

    // 設定
    Config config_;

    // 補間のための値
    float currentValue_ = 0.0f;
};