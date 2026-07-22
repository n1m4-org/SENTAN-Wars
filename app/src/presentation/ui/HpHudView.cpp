#include "HpHudView.h"
#include <common/ResourcePath.h>
#include <SpriteManager.h>


HpHudView::HpHudView()
{
    this->InitializeSprites();
    this->InitializeFlexContainer();
    this->ApplyFlexLayout();
    this->RegisterCustomGui();
}

void HpHudView::Update()
{
    this->ApplyFlexLayout();
    bar_.Update(currentHp_, maxHp_);
}

void HpHudView::InitializeSprites()
{
    pSpriteHp_ = std::make_unique<Hagine::Sprite>();
    pSpriteHp_->Initialize(Path::Image::Hp, {});

    /// HPバーの初期化
    Bar2d::Config cfg;
    cfg.colorWarn = 0xffff00ff;
    cfg.interpolationRatio = 1.0f;
    cfg.maxSize = { 296.0f, 16.0f };

    BarMovable::Config diff = {};
    diff.color = RGBA(0xcdcdcdff);
    diff.interpolationRatio = 0.03f;
    diff.maxSize = cfg.maxSize;

    cfg.diff = diff;

    bar_.Initialize(cfg);

    /// スプライトマネージャに登録
    auto sm = Hagine::SpriteManager::GetInstance();
    sm->RegisterExternal(pSpriteHp_.get());

    /// フレックスアイテムの初期化
    flexItems_.push_back({ pSpriteHp_->GetSize() });
    flexItems_.push_back({ bar_.GetSize() });
}

void HpHudView::InitializeFlexContainer()
{
    pFlexContainer_ = std::make_unique<FlexContainer>();
    pFlexContainer_->direction_ = FlexDirection::Row;
    pFlexContainer_->justifyContent_ = JustifyContent::Center;
    pFlexContainer_->alignItems_ = AlignItems::Center;
    pFlexContainer_->gap_ = 32.0f;

    // デバッグ用の名前を設定
    pFlexContainer_->SetName("HpHud");

    // コンテナの位置とサイズを設定
    containerBox_.position = Hagine::Vector2(0.0f, 860.0f);
    containerBox_.size = Hagine::Vector2(500.0f, 100.0f);
}

void HpHudView::ApplyFlexLayout()
{
    auto result = pFlexContainer_->Calculate(containerBox_, flexItems_);

    pSpriteHp_->SetPosition(result[0].position);
    pSpriteHp_->SetSize(result[0].size);

    bar_.SetPosition(result[1].position);
}

void HpHudView::RegisterCustomGui()
{
#ifdef _DEBUG
    auto pFunc = [this]()
    {
        if (ImGui::Button("Sub 1HP"))
        {
            currentHp_ -= 1.0f;
        }

        if (ImGui::Button("Sub 5HP"))
        {
            currentHp_ -= 5.0f;
        }

        if (ImGui::Button("Sub 10HP"))
        {
            currentHp_ -= 10.0f;
        }

        if (ImGui::Button("Sub 50HP"))
        {
            currentHp_ -= 50.0f;
        }

        if (ImGui::Button("Add 1HP"))
        {
            currentHp_ += 1.0f;
        }

        if (ImGui::Button("Add 5HP"))
        {
            currentHp_ += 5.0f;
        }

        if (ImGui::Button("Add 10HP"))
        {
            currentHp_ += 10.0f;
        }

        if (ImGui::Button("Add 50HP"))
        {
            currentHp_ += 50.0f;
        }
    };

    debugEntry.RegisterCustomGuiFunction("", pFunc);
#endif //_DEBUG
}
