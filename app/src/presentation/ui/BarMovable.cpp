#include "BarMovable.h"
#include <common/ResourcePath.h>
#include <SpriteManager.h>
#include <algorithm>


void BarMovable::Initialize(const Config& cfg)
{
    config_ = cfg;
    InitializeSprites(config_.color);
}

void BarMovable::Update(float value, float valueMax)
{
    // スプライトの位置を更新
    pSpriteBar_->SetPosition(leftTop_);

    // 補間の計算
    if (config_.interpolationRatio.has_value())
    {
        currentValue_ = std::lerp(currentValue_, value, config_.interpolationRatio.value());
    }

    // 補間後の値が最大値に近い場合は、最大値に設定する
    if (valueMax - currentValue_ < 0.01f)
    {
        currentValue_ = valueMax;
    }

    // 値を0～valueMaxの範囲にクランプ
    float clampedValue = std::clamp(currentValue_, 0.0f, valueMax);

    // HPバーのサイズを更新
    // テクスチャサイズを変更してバーの長さを変える方法
    pSpriteBar_->SetSize({ config_.maxSize.x * (clampedValue / valueMax), config_.maxSize.y });

    // 警告色の適用
    if (config_.colorWarn.has_value())
    {
        if (config_.warnThreshold && value / valueMax <= config_.warnThreshold)
        {
            pSpriteBar_->SetColor(config_.colorWarn.value().to_Vector3());
        }
        else
        {
            pSpriteBar_->SetColor(config_.color.to_Vector3());
        }
    }
}

Hagine::Vector2 BarMovable::GetSize() const
{
    if (pSpriteBar_)
    {
        return pSpriteBar_->GetSize();
    }
    return Hagine::Vector2(0.0f, 0.0f);
}

void BarMovable::SetColor(RGBA color)
{
    pSpriteBar_->SetColor(color.to_Vector3());
}

RGBA BarMovable::GetColor() const
{
    return RGBA(pSpriteBar_->GetColor());
}

void BarMovable::InitializeSprites(RGBA color)
{
    pSpriteBar_ = std::make_unique<Hagine::Sprite>();
    pSpriteBar_->Initialize(Path::Image::Bar, {});
    pSpriteBar_->SetColor(color.to_Vector3());

    // スプライトマネージャに登録
    auto sm = Hagine::SpriteManager::GetInstance();
    sm->RegisterExternal(pSpriteBar_.get());
}
