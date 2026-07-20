#include "WaveCountHudView.h"
#include <common/ResourcePath.h>
#include <WinApp.h>
#include <utility/ViewportUnits.hpp>
#include <SpriteManager.h>


WaveCountHudView::WaveCountHudView()
{
    // コンテナの位置を設定
    containerBox_.position =
    {
        0.0f,
        4_vh
    };

    // コンテナのサイズを設定
    containerBox_.size =
    {
        static_cast<float>(Hagine::WinApp::GetVirtualWidth()),
        static_cast<float>(5_vh)
    };

    this->InitializeFlexContainer();
    this->InitializeNumericView();
}

void WaveCountHudView::Update()
{
    pNumericView_->Update();
    this->UpdateFlexItems();

    auto flexResult = pFlexContainer_->Calculate(containerBox_, flexItems_);
    pWaveSprite_->SetPosition(flexResult[0].position);
    pNumericView_->GetFontLayoutProperties().leftTop = flexResult[1].position;
}

void WaveCountHudView::InitializeFlexContainer()
{
    pFlexContainer_ = std::make_unique<FlexContainer>();
    pFlexContainer_->direction_ = FlexDirection::Row;
    pFlexContainer_->justifyContent_ = JustifyContent::Center;
    pFlexContainer_->alignItems_ = AlignItems::Center;
    pFlexContainer_->gap_ = 32.0f;
}

void WaveCountHudView::InitializeNumericView()
{
    pNumericView_ = std::make_unique<NumericView>();
    std::vector<std::string> textureHandles;
    for (const auto& path : Path::Image::Numbers)
    {
        textureHandles.emplace_back(path);
    }
    pNumericView_->Initialize(textureHandles, "WaveCount");

    pWaveSprite_ = std::make_unique<Hagine::Sprite>();
    pWaveSprite_->Initialize(Path::Image::Wave, {});
    Hagine::SpriteManager::GetInstance()->RegisterExternal(pWaveSprite_.get());
}

void WaveCountHudView::UpdateFlexItems()
{
    flexItems_.clear();
    flexItems_.emplace_back(FlexItem{ pWaveSprite_->GetSize() });
    flexItems_.emplace_back(FlexItem{ pNumericView_->GetSizeOverall() });
}
