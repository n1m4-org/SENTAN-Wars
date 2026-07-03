#include "WaveDirector.h"
#include "GeneralWave.h"



void WaveDirector::Initialize()
{
    // デバッグ用のパラメータの変更コールバックを登録
    this->RegisterOnChange();

    // 初期ウェーブを作成
    this->ChangeToNextWave();
}

void WaveDirector::Update()
{
    
}

WaveType WaveDirector::GetWaveType(uint32_t waveIndex) const
{
    const uint32_t kBossWaveIndexPerLap = numGeneralWavePerLap_;
    if (waveIndex % kBossWaveIndexPerLap == 0 && waveIndex != 0)
    {
        return WaveType::Boss;
    }
    return WaveType::General;
}

std::unique_ptr<IWave> WaveDirector::CreateWave(WaveType type)
{
    switch (type)
    {
    case WaveType::General:
        return std::make_unique<GeneralWave>();
        break;
    case WaveType::Boss:
        return nullptr;
        break;
    default:
        return nullptr;
        break;
    }
}

void WaveDirector::RegisterOnChange()
{
    debug_goNextWave_.SetOnChange([this](const bool& value)
    {
        if (value)
        {
            this->ChangeToNextWave();
            goNextWave_ = false;
        }
    });
}

void WaveDirector::ChangeToNextWave()
{
    ++waveIndex_;
    WaveType nextWaveType = this->GetWaveType(waveIndex_);
    pCurrentWave_ = this->CreateWave(nextWaveType);
}
