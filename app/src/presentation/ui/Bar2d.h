#pragma once
#include <memory>
#include <Sprite.h>
#include <type/Vector2.h>
#include <math/Color.h>
#include <optional>
#include "BarMovable.h"


class Bar2d
{
public:
    struct Config
    {
        // メイン色
        RGBA colorMain = 0xa82227ff;

        // 背景色
        RGBA colorBg = 0x313131ff;

        // 警告色（std::nulloptの場合は警告色を使用しない）
        std::optional<RGBA> colorWarn = std::nullopt;

        // 警告色に切り替わる閾値（0.0f～1.0f）
        float warnThreshold = 0.2f;

        // 補間の速度（0.0f～1.0f）
        float interpolationRatio = 1.0f;

        Hagine::Vector2 maxSize = { 100.0f, 10.0f };

        std::optional<BarMovable::Config> main = std::nullopt;
        std::optional<BarMovable::Config> diff = std::nullopt;
    };

    void Initialize(const Config& cfg);
    void Update(float value, float valueMax);

    Hagine::Vector2 GetPosition() const { return pMainBar_->GetPosition(); }
    void SetPosition(const Hagine::Vector2& position);

    Hagine::Vector2 GetSize() const;

private:
    // スプライトの初期化
    void InitializeSprites();

    // 設定を適用
    void ApplyConfig(BarMovable::Config& cfg, BarMovable::Config* pIndividual);

    // メインバーのスプライト
    std::unique_ptr<BarMovable> pMainBar_ = nullptr;

    // 差分バーのスプライト
    std::unique_ptr<BarMovable> pSpriteDiffBar_ = nullptr;

    // HPバーの背景スプライト
    std::unique_ptr<Hagine::Sprite> pSpriteBarBg_ = nullptr;

    // 座標
    Hagine::Vector2 leftTop_ = { 0.0f, 0.0f };

    // 設定
    Config config_;

    // 補間のための値
    float currentValue_ = 0.0f;

    std::unique_ptr<BarMovable> pBar2dDiff_ = nullptr;
};