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


    uint32_t GetWaveIndex() const override { return waveIndex_; }


    bool IsWaveFinished() const override;


    bool IsEnemyBudgetExhausted() const override;

private:
    uint32_t waveIndex_;
    std::unique_ptr<PhaseDirector> pPhaseDirector_;
};