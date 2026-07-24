#include "SentanSelectCard.h"
#include <common/ResourcePath.h>
#include <SpriteManager.h>
#include <algorithm>


SentanSelectCard::SentanSelectCard(SentanId sentanId) : sentanId_(sentanId)
{
    this->InitializeSprites();
}

SentanSelectCard::~SentanSelectCard()
{
    // 描画リストは持ち主を所有しないので、消える前に自分で外す
    this->Unregister();
}

void SentanSelectCard::Unregister()
{
    auto sm = Hagine::SpriteManager::GetInstance();
    sm->UnregisterExternal(pIcon_.get());
    sm->UnregisterExternal(pCardBg_.get());
}

void SentanSelectCard::Update(bool isSelected)
{
    // カードとアイコンは同じ見た目の状態にする（片方だけ明るいと選べるように見えてしまう）
    Hagine::Vector3 color = { 0.5f, 0.5f, 0.5f }; // 選択されていない場合は灰色

    if (isTaken_)
    {
        color = { 0.25f, 0.25f, 0.25f }; // 選び終えた場合は暗くする
    }
    else if (isSelected)
    {
        color = { 1.0f, 1.0f, 1.0f }; // 選択されている場合は白色
    }

    pCardBg_->SetColor(color);
    pIcon_->SetColor(color);
}

void SentanSelectCard::SetPosition(const Hagine::Vector2& position)
{
    pCardBg_->SetPosition(position);

    // アイコンはカードの中心に置く（アンカーが中心なので、渡した位置がそのまま中心になる）
    const auto cardSize = pCardBg_->GetSize();
    pIcon_->SetPosition({ position.x + cardSize.x / 2.0f, position.y + cardSize.y / 2.0f });
}

void SentanSelectCard::InitializeSprites()
{
    pCardBg_ = std::make_unique<Hagine::Sprite>();
    pCardBg_->Initialize(Path::Image::CardBg, {});

    pIcon_ = std::make_unique<Hagine::Sprite>();
    pIcon_->Initialize(this->GetPath(sentanId_), {});

    // アイコンの画像はカードより大きいので、縦横比を保ったままカードの内側へ収める
    // カードに合わせて縮めないと、選択の枠とSENTANの絵がずれて見える
    const Hagine::Vector2 cardSize = pCardBg_->GetSize();
    const Hagine::Vector2 iconSize = pIcon_->GetSize();
    if (iconSize.x > 0.0f && iconSize.y > 0.0f)
    {
        const float scale = (std::min)((cardSize.x - kIconPadding_ * 2.0f) / iconSize.x,
                                       (cardSize.y - kIconPadding_ * 2.0f) / iconSize.y);
        pIcon_->SetSize({ iconSize.x * scale, iconSize.y * scale });
    }

    // 中心を基準にして、カードの中心と合わせられるようにする
    pIcon_->SetAnchorPoint({ 0.5f, 0.5f });

    auto sm = Hagine::SpriteManager::GetInstance();
    sm->RegisterExternal(pCardBg_.get());
    sm->RegisterExternal(pIcon_.get());
}
