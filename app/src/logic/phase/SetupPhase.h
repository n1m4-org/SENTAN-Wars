#pragma once

#include "IPhase.h"
#include <entity/WarpHole.h>

class SetupPhase : public IPhase
{
public:
    void Enter() override;


    void Exit() override;


    void Update() override;


    void Draw() override;

private:
    std::unique_ptr<WarpHole> pWarpHole_ = nullptr;
};