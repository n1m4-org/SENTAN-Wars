#pragma once

#include "IWave.h"
#include <cstdint>

class GeneralWave : public IWave
{
public:


    void Enter(const WaveContext& ctx) override;


    void Exit() override;


    void Update() override;


    uint32_t GetWaveIndex() const override { return waveIndex_; }

private:
    uint32_t waveIndex_;
};