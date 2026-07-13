#include "BossWave.h"
#include <stdexcept>



void BossWave::Enter(const WaveContext& ctx)
{
    pPhaseDirector_ = std::make_unique<PhaseDirector>();
    pPhaseDirector_->Initialize();
}

void BossWave::Exit()
{
    pPhaseDirector_->Finalize();
}

void BossWave::Update()
{
    pPhaseDirector_->Update();
}

void BossWave::Draw()
{
    pPhaseDirector_->Draw();
}

uint32_t BossWave::GetWaveIndex() const
{
    throw std::logic_error("The method or operation is not implemented.");
}

bool BossWave::IsWaveFinished() const
{
    throw std::logic_error("The method or operation is not implemented.");
}

bool BossWave::IsEnemyBudgetExhausted() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
