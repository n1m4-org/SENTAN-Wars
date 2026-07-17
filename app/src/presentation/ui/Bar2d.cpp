#include "Bar2d.h"
#include <common/ResourcePath.h>
#include <SpriteManager.h>
#include <algorithm>


void Bar2d::Initialize(const Config& cfg)
{
    config_ = cfg;
    InitializeSprites(config_.colorMain, config_.colorBg);
}

void Bar2d::Update(float value, float valueMax)
{
    // スプライトの位置を更新
    pSpriteBarBg_->SetPosition(leftTop_);
    pSpriteBar_->SetPosition(leftTop_);

    // 補間の進行度を更新
    auto sizeMax = pSpriteBarBg_->GetSize();

    // 補間の計算
    currentValue_ = std::lerp(currentValue_, value, config_.interpolationRatio);
    
    // 補間後の値が最大値に近い場合は、最大値に設定する
    if (valueMax - currentValue_ < 0.01f)
    {
        currentValue_ = valueMax;
    }

    // 値を0～valueMaxの範囲にクランプ
    float clampedValue = std::clamp(currentValue_, 0.0f, valueMax);

    // HPバーのサイズを更新
    pSpriteBar_->SetSize({ sizeMax.x * (clampedValue / valueMax), sizeMax.y });

    // 警告色の適用
    if (config_.warnColor.has_value())
    {
        if (config_.warnThreshold && value / valueMax <= config_.warnThreshold)
        {
            pSpriteBar_->SetColor(config_.warnColor.value().to_Vector3());
        }
        else
        {
            pSpriteBar_->SetColor(config_.colorMain.to_Vector3());
        }
    }
}

Hagine::Vector2 Bar2d::GetSize() const
{
    if (pSpriteBar_)
    {
        return pSpriteBar_->GetSize();
    }
    return Hagine::Vector2(0.0f, 0.0f);
}

void Bar2d::SetMainColor(RGBA color)
{
    pSpriteBar_->SetColor(color.to_Vector3());
}

RGBA Bar2d::GetMainColor() const
{
    return RGBA(pSpriteBar_->GetColor());
}

void Bar2d::SetBackgroundColor(RGBA color)
{
    pSpriteBarBg_->SetColor(color.to_Vector3());
}

RGBA Bar2d::GetBackgroundColor() const
{
    return RGBA(pSpriteBarBg_->GetColor());
}

void Bar2d::InitializeSprites(RGBA colorMain, RGBA colorBg)
{
    pSpriteBarBg_ = std::make_unique<Hagine::Sprite>();
    pSpriteBarBg_->Initialize(Path::Image::Bar, {});
    pSpriteBarBg_->SetColor(colorBg.to_Vector3());

    pSpriteBar_ = std::make_unique<Hagine::Sprite>();
    pSpriteBar_->Initialize(Path::Image::Bar, {});
    pSpriteBar_->SetColor(colorMain.to_Vector3());

    // スプライトマネージャに登録
    auto sm = Hagine::SpriteManager::GetInstance();
    sm->RegisterExternal(pSpriteBarBg_.get());
    sm->RegisterExternal(pSpriteBar_.get());
}
