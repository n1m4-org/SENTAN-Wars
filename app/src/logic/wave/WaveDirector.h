#pragma once
#include <debug/GameParameter.h>
#include "IWave.h"

class WaveDirector
{
public:
    void Initialize();
    void Update();

private:
    // 現在のウェーブ
    std::unique_ptr<IWave> currentWave_;

    /// パラメータ
    EnableDebug("WaveDirector");
    GameParameter(uint32_t, waveIndex_, 0u);
};