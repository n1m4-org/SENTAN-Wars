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


    uint32_t GetWaveIndex() const override;


    bool IsWaveFinished() const override;


    bool IsEnemyBudgetExhausted() const override;

private:
    std::unique_ptr<PhaseDirector> pPhaseDirector_;
    uint32_t waveIndex_;

};