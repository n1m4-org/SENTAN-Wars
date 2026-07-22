#include "Bar2d.h"
#include <common/ResourcePath.h>
#include <SpriteManager.h>

void Bar2d::Initialize(const Config& cfg)
{
    config_ = cfg;
    this->InitializeSprites();
}

void Bar2d::Update(float value, float valueMax)
{
    // 背景スプライトの位置を更新
    pSpriteBarBg_->SetPosition(leftTop_);
    pMainBar_->SetPosition(leftTop_);

    pMainBar_->Update(value, valueMax);
}

void Bar2d::SetPosition(const Hagine::Vector2& position)
{
    leftTop_ = position;
}

Hagine::Vector2 Bar2d::GetSize() const
{
    return pSpriteBarBg_->GetSize();
}

void Bar2d::InitializeSprites()
{
    auto sm = Hagine::SpriteManager::GetInstance();

    /// 背景スプライトの初期化
    pSpriteBarBg_ = std::make_unique<Hagine::Sprite>();
    pSpriteBarBg_->Initialize(Path::Image::Bar, {});
    pSpriteBarBg_->SetColor(config_.colorBg.to_Vector3());
    pSpriteBarBg_->SetSize(config_.maxSize);
    sm->RegisterExternal(pSpriteBarBg_.get());

    /// メインバーの初期化
    BarMovable::Config mainBarCfg = {};
    if (config_.main.has_value())
    {
        ApplyConfig(mainBarCfg, &config_.main.value());
    }
    else ApplyConfig(mainBarCfg, nullptr);
    
    pMainBar_ = std::make_unique<BarMovable>();
    pMainBar_->Initialize(mainBarCfg);
}

void Bar2d::ApplyConfig(BarMovable::Config& cfg, BarMovable::Config* pIndividual)
{
    if (pIndividual)
    {
        cfg = *pIndividual;
    }
    else
    {
        // 親の設定を継承
        cfg.color = config_.colorMain;
        cfg.colorWarn = config_.colorWarn;
        cfg.warnThreshold = config_.warnThreshold;
        cfg.interpolationRatio = config_.interpolationRatio;
        cfg.maxSize = config_.maxSize;
    }
}
