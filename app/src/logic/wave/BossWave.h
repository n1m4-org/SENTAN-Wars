#pragma once

#include "IWave.h"
#include <logic/phase/PhaseDirector.h>

class BossWave : public IWave
{
public:
    void Enter(const WaveContext& ctx) override;


    void Exit() override;


    void Update() override;


    void Draw() override;


    bool IsWaveFinished() const override;


    bool IsEnemyBudgetExhausted() const override;

private:
    std::unique_ptr<PhaseDirector> pPhaseDirector_;
    WaveContext ctx_;
};