#pragma once
#include <memory>
#include <Sprite.h>
#include <type/Vector2.h>
#include <math/Color.h>
#include <optional>
#include "BarMovable.h"


class BarMovable
{
public:
    struct Config
    {
        RGBA color = 0xa82227ff;

        // 警告色（std::nulloptの場合は警告色を使用しない）
        std::optional<RGBA> colorWarn = std::nullopt;

        // 警告色に切り替わる閾値（0.0f～1.0f）
        float warnThreshold = 0.2f;

        // 補間の速度（0.0f～1.0f） (個別設定用)
        std::optional<float> interpolationRatio = std::nullopt;

        Hagine::Vector2 maxSize = { 100.0f, 10.0f };
    };

    void Initialize(const Config& cfg);
    void Update(float value, float valueMax);

    Hagine::Vector2 GetPosition() const { return leftTop_; }
    void SetPosition(const Hagine::Vector2& position) { leftTop_ = position; }

    Hagine::Vector2 GetSize() const;

    void SetColor(RGBA color);
    RGBA GetColor() const;

private:
    // スプライトの初期化
    void InitializeSprites(RGBA color);

    // バーのスプライト
    std::unique_ptr<Hagine::Sprite> pSpriteBar_ = nullptr;

    // 座標
    Hagine::Vector2 leftTop_ = { 0.0f, 0.0f };

    // 設定
    Config config_;

    // 補間のための値
    float currentValue_ = 0.0f;
};