#pragma once

#include "IPhase.h"
#include <debugapi.h>

class BossPhase : public IPhase
{
public:
    void Enter() override
    {
        OutputDebugStringA("BossPhase::Enter\n");
    }


    void Exit() override
    {
        OutputDebugStringA("BossPhase::Exit\n");
    }


    void Update() override
    {
        OutputDebugStringA("BossPhase::Update\n");
    }


    void Draw() override
    {
        OutputDebugStringA("BossPhase::Draw\n");
    }

};