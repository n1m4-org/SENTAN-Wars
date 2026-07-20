#pragma once
#include "IHudView.h"
#include <memory>
#include <font/NumericView.h>


class WaveCountHudView : public IHudView
{
public:
    WaveCountHudView();
	void Update() override;

private:
    void InitializeNumericView();
    std::unique_ptr<NumericView> pNumericView_ = nullptr;
};