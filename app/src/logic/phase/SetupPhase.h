#pragma once

#include "IPhase.h"
#include <debugapi.h>

class SetupPhase : public IPhase
{
public:
    void Enter() override
    {
        OutputDebugStringA("SetupPhase::Enter\n");
    }

    void Exit() override
    {
        OutputDebugStringA("SetupPhase::Exit\n");
    }

    void Update() override
    {
        OutputDebugStringA("SetupPhase::Update\n");
    }

    void Draw() override
    {
        OutputDebugStringA("SetupPhase::Draw\n");
    }
};