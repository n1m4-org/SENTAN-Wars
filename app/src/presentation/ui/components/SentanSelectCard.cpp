#include "SentanSelectCard.h"
#include <common/ResourcePath.h>
#include <SpriteManager.h>


SentanSelectCard::SentanSelectCard(SentanId sentanId) : sentanId_(sentanId)
{
    this->InitializeSprites();
}

void SentanSelectCard::Update(bool isSelected)
{
    if (isSelected)
    {
        pCardBg_->SetColor({ 1.0f, 1.0f, 1.0f }); // 選択されている場合は白色
    }
    else
    {
        pCardBg_->SetColor({ 0.5f, 0.5f, 0.5f }); // 選択されていない場合は灰色
    }
}

void SentanSelectCard::SetPosition(const Hagine::Vector2& position)
{
    pCardBg_->SetPosition(position);
    
    const auto size = pCardBg_->GetSize();
    pIcon_->SetPosition({ position.x + size.x / 2.0f, position.y + 10.0f }); // アイコンの位置を調整
}

void SentanSelectCard::InitializeSprites()
{
    pCardBg_ = std::make_unique<Hagine::Sprite>();
    pCardBg_->Initialize(Path::Image::CardBg, {});

    pIcon_ = std::make_unique<Hagine::Sprite>();
    pIcon_->Initialize(this->GetPath(sentanId_), {});

    auto sm = Hagine::SpriteManager::GetInstance();
    sm->RegisterExternal(pCardBg_.get());
    sm->RegisterExternal(pIcon_.get());
}
