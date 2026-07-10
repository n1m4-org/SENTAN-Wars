#include "GeneralWave.h"



void GeneralWave::Enter(const WaveContext& ctx)
{
    pPhaseDirector_ = std::make_unique<PhaseDirector>();
    pPhaseDirector_->Initialize();
    pPhaseDirector_->ChangePhase(PhaseType::Battle);
}

void GeneralWave::Exit()
{
    pPhaseDirector_->Finalize();
}

void GeneralWave::Update()
{
    pPhaseDirector_->Update();
}

void GeneralWave::Draw()
{
    pPhaseDirector_->Draw();
}

bool GeneralWave::IsWaveFinished() const
{
    // TODO: 終了条件を実装する
    return false;
}

bool GeneralWave::IsEnemyBudgetExhausted() const
{
    // TODO: 予算が尽きたかどうかの判定を実装する
    return false;
}
