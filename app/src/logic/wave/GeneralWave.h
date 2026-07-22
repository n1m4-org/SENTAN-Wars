#pragma once

#include "IWave.h"
#include <cstdint>
#include <logic/phase/PhaseDirector.h>
#include <memory>

class GeneralWave : public IWave
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
};