#include "ResultFrame.h"
#include <algorithm>
#include <common/ResourcePath.h>
#include <SpriteManager.h>
#include <utility/SpriteUnregisterer.h>

using namespace Hagine;

void ResultFrame::Initialize(const Config& config)
{
    config_ = config;
    phase_ = Phase::Horizontal;
    timer_ = 0.0f;

    // 縁取り（灰色の矩形）
    pBorder_ = std::make_unique<Sprite>();
    pBorder_->Initialize(Path::Image::kWhite1x1, config_.center, config_.borderColor, { 0.5f, 0.5f });

    // 内側（半透明の黒）
    pInner_ = std::make_unique<Sprite>();
    pInner_->Initialize(Path::Image::kWhite1x1, config_.center, config_.innerColor, { 0.5f, 0.5f });

    // 展開開始時は横方向の細い線から始める
    this->ApplyToSprites({ 0.0f, config_.lineThickness });

    // スプライトマネージャに登録（縁取り → 内側 の順で登録し、内側を前面に描画する）
    SpriteManager* sm = SpriteManager::GetInstance();
    sm->RegisterExternal(pBorder_.get());
    sm->RegisterExternal(pInner_.get());
}

void ResultFrame::Finalize()
{
    Sprite* sprites[] = { pBorder_.get(), pInner_.get() };
    utl::sprite::Unregister(sprites);
}

void ResultFrame::Update(float deltaTime)
{
    switch (phase_)
    {
    case Phase::Horizontal:
    {
        // 横方向に展開（高さは線の太さで固定）
        timer_ = (std::min)(timer_ + deltaTime, config_.horizontalDuration);
        const float width = ApplyEasing<float>(config_.horizontalEasing, 0.0f, config_.size.x, timer_, config_.horizontalDuration);
        this->ApplyToSprites({ width, config_.lineThickness });

        if (timer_ >= config_.horizontalDuration)
        {
            phase_ = Phase::Vertical;
            timer_ = 0.0f;
        }
        break;
    }
    case Phase::Vertical:
    {
        // 縦方向に展開して長方形を形成する
        timer_ = (std::min)(timer_ + deltaTime, config_.verticalDuration);
        const float height = ApplyEasing<float>(config_.verticalEasing, config_.lineThickness, config_.size.y, timer_, config_.verticalDuration);
        this->ApplyToSprites({ config_.size.x, height });

        if (timer_ >= config_.verticalDuration)
        {
            phase_ = Phase::Finished;
        }
        break;
    }
    case Phase::Finished:
        this->ApplyToSprites(config_.size);
        break;
    }
}

void ResultFrame::Skip()
{
    phase_ = Phase::Finished;
    timer_ = 0.0f;
    this->ApplyToSprites(config_.size);
}

void ResultFrame::ApplyToSprites(const Vector2& currentSize)
{
    // 縁取り（外側の灰色矩形）
    pBorder_->SetPosition(config_.center);
    pBorder_->SetSize(currentSize);

    // 内側（縁取りの太さ分だけ内側に縮めた半透明の黒）
    const Vector2 innerSize = {
        (std::max)(0.0f, currentSize.x - config_.borderThickness * 2.0f),
        (std::max)(0.0f, currentSize.y - config_.borderThickness * 2.0f)
    };
    pInner_->SetPosition(config_.center);
    pInner_->SetSize(innerSize);
}
