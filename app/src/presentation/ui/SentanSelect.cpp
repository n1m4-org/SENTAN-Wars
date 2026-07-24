#include "SentanSelect.h"
#include <common/ResourcePath.h>
#include <utility/ViewportUnits.hpp>
#include <SpriteManager.h>
#include <logic/EquippedSentanCache.h>
#include <algorithm>
#include <random>
#include <vector>



SentanSelect::SentanSelect(uint32_t pickCount)
{
    // 出せるカードより多くは選ばせられない
    pickCount_ = (pickCount < cards_.size()) ? pickCount : static_cast<uint32_t>(cards_.size());

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
    // 選び終わった後は入力を受け付けない（同じキーで余計に決まらないようにする）
    if (this->IsDecided()) return;

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

    if (!inputData.isConfirmed) return;

    // すでに選んだカードは選び直せない
    const uint32_t index = inputData.selectedIndex;
    if (isTaken_[index]) return;

    isTaken_[index] = true;
    cards_[index]->SetTaken(true);
    ++decidedCount_;

    // 選ばれたSENTANを外へ渡す（装備するかどうかはこのクラスが決めない）
    if (onDecideCallback_)
    {
        onDecideCallback_(pickedIds_[index]);
    }

    // まだ選ぶ枚数が残っていれば、次に選べるカードへカーソルを移す
    if (!this->IsDecided())
    {
        pInput_->SetSelectedIndex(this->FindSelectableIndex(index));
    }
}

uint32_t SentanSelect::FindSelectableIndex(uint32_t index) const
{
    const uint32_t count = static_cast<uint32_t>(cards_.size());
    for (uint32_t offset = 1; offset < count; ++offset)
    {
        const uint32_t next = (index + offset) % count;
        if (!isTaken_[next])
        {
            return next;
        }
    }
    return index;
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
    const auto* esCache = EquippedSentanCache::GetInstance();

    // まだ持っていないSENTANを候補として集める
    // すでに持っているものを出すと、選ばれたときに同じ振る舞いが二重に付いてしまう
    std::vector<SentanId> candidates;
    for (int i = static_cast<int>(SentanId::Sentan1); i <= static_cast<int>(SentanId::Sentan5); ++i)
    {
        const SentanId id = static_cast<SentanId>(i);
        if (!esCache->IsEquipped(id))
        {
            candidates.push_back(id);
        }
    }

    // 候補が枠に足りないときは、持っているものも並べて埋める（枠が空くよりはまし）
    for (int i = static_cast<int>(SentanId::Sentan1);
         candidates.size() < pickedIds_.size() && i <= static_cast<int>(SentanId::Sentan5); ++i)
    {
        candidates.push_back(static_cast<SentanId>(i));
    }

    // 並びをばらして先頭から取る（同じSENTANが2枚並ばない）
    std::shuffle(candidates.begin(), candidates.end(), std::mt19937{std::random_device{}()});
    for (uint32_t i = 0; i < pickedIds_.size(); ++i)
    {
        pickedIds_[i] = candidates[i];
    }
}
