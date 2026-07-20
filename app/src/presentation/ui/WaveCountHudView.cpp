#include "WaveCountHudView.h"
#include <common/ResourcePath.h>



WaveCountHudView::WaveCountHudView()
{
    this->InitializeNumericView();
}

void WaveCountHudView::Update()
{
    pNumericView_->Update();
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
}
