#include "HpHudView.h"
#include <common/ResourcePath.h>


void HpHudView::Initialize()
{
    this->InitializeSprites();
    this->InitializeFlexContainer();
    this->ApplyFlexLayout();
    this->RegisterCustomGui();
}

void HpHudView::Update(float hp, float hpMax)
{
    this->ApplyFlexLayout();
    bar_.Update(currentHp_, hpMax);
}

void HpHudView::InitializeSprites()
{
    pSpriteHp_ = std::make_unique<Hagine::Sprite>();
    pSpriteHp_->Initialize(Path::Image::Hp, {});

    Bar2d::Config cfg;
    cfg.warnColor = 0xffff00ff;
    cfg.interpolationRatio = 0.1f;
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
}

void HpHudView::ApplyFlexLayout()
{
    auto result = pFlexContainer_->Calculate(containerSize_, flexItems_);

    result[0].position += containerPosition_;
    result[1].position += containerPosition_;

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
