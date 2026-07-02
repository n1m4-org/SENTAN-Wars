#pragma once
#include <debug/GameParameter.h>
#include <memory>
#include "IWave.h"
#include <cstdint>

enum class WaveType
{
    General,
    Boss,
};

class WaveDirector
{
public:
    void Initialize();
    void Update();

private:
    std::unique_ptr<IWave> CreateWave(WaveType type);

    // 現在のウェーブ
    std::unique_ptr<IWave> currentWave_;

    /// パラメータ
    EnableDebug("WaveDirector");
    GameParameter(int, waveIndex_, 0);
};