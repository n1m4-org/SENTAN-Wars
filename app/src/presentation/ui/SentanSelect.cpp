#include "SentanSelect.h"
#include <common/ResourcePath.h>
#include <utility/ViewportUnits.hpp>
#include <SpriteManager.h>
#include <logic/SentanRandomPicker.h>
#include <logic/EquippedSentanCache.h>



SentanSelect::SentanSelect()
{
    this->RandomPickSentanIds();
    this->InitializeSprite();
    this->InitializeComponents();
    this->InitializeCards();
    this->InitializeContainer();
    this->ApplyLayout();
}

SentanSelect::~SentanSelect()
{
    Hagine::SpriteManager::GetInstance()->UnregisterExternal(pSpriteWindow_.get());
}

void SentanSelect::Update()
{
    /// 選択肢の更新
    auto inputData = pInput_->Update();
    for (uint32_t i = 0; i < cards_.size(); ++i)
    {
        if (inputData.selectedIndex == i)
        {
            cards_[i]->Update(true);
        }
        else
        {
            cards_[i]->Update(false);
        }
    }
}

void SentanSelect::InitializeSprite()
{
    pSpriteWindow_ = std::make_unique<Hagine::Sprite>();
    pSpriteWindow_->Initialize(Path::Image::SelectWindow, { 50_vw, 50_vh });
    pSpriteWindow_->SetAnchorPoint({ 0.5f, 0.5f });

    Hagine::SpriteManager::GetInstance()->RegisterExternal(pSpriteWindow_.get());
}

void SentanSelect::InitializeCards()
{
    for (uint32_t i = 0; i < cards_.size(); ++i)
    {
        cards_[i] = std::make_unique<SentanSelectCard>(pickedIds_[i]);
    }
}

void SentanSelect::InitializeComponents()
{
    pInput_ = std::make_unique<SentanSelectInput>();
}

void SentanSelect::InitializeContainer()
{
    flexContainer_.direction_ = FlexDirection::Row;
    flexContainer_.justifyContent_ = JustifyContent::SpaceEvenly;
    flexContainer_.alignItems_ = AlignItems::Center;
    flexContainer_.gap_ = 56.0f;
}

void SentanSelect::ApplyLayout()
{
    for (uint32_t i = 0; i < cards_.size(); ++i)
    {
        flexItems_[i].preferredSize = cards_[i]->GetSize();
    }

    Hagine::Vector2 containerSize = flexContainer_.ContainerSize(flexItems_);
    flexBox_.size = containerSize;
    flexBox_.position = { 50_vw - containerSize.x / 2.0f, 50_vh - containerSize.y / 2.0f };

    auto result = flexContainer_.Calculate(flexBox_, flexItems_);

    for (uint32_t i = 0; i < cards_.size(); ++i)
    {
        cards_[i]->SetPosition(result[i].position);
    }
}

void SentanSelect::RandomPickSentanIds()
{
    auto esCache = EquippedSentanCache::GetInstance();

    if (esCache->IsEquippedAll()) return;

    bool isEquippedAll = true;
    do
    {
        isEquippedAll = true;
        for (auto& id : pickedIds_)
        {
            id = PickRandomSentanId();
        }

        for (const auto& id : pickedIds_)
        {
            isEquippedAll &= esCache->IsEquipped(id);
        }
    }
    while (isEquippedAll);
}
