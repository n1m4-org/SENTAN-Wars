#include "WaveDirector.h"
#include "GeneralWave.h"



void WaveDirector::Initialize()
{

}

void WaveDirector::Update()
{

}

std::unique_ptr<IWave> WaveDirector::CreateWave(WaveType type)
{
    switch (type)
    {
    case WaveType::General:
        return std::make_unique<GeneralWave>();
        break;
    case WaveType::Boss:
        return nullptr; // TODO: Implement BossWave
        break;
    default:
    }
}
