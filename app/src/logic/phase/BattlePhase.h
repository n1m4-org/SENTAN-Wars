#pragma once
#include "IPhase.h"

#include <debugapi.h>

class BattlePhase : public IPhase
{
public:
    void Enter() override
    {
        OutputDebugStringA("BattlePhase::Enter\n");
    }
    void Exit() override
    {
        OutputDebugStringA("BattlePhase::Exit\n");
    }
    void Update() override
    {
        OutputDebugStringA("BattlePhase::Update\n");
    }
    void Draw() override
    {
        OutputDebugStringA("BattlePhase::Draw\n");
    }
};