#include "GeneralWave.h"
#include "Character/Enemy/EnemyManager.h"
#include "logic/phase/BattlePhase.h"

void GeneralWave::Enter(const WaveContext& ctx)
{
    ctx_ = ctx;
    pPhaseDirector_ = std::make_unique<PhaseDirector>();
    pPhaseDirector_->Initialize();
    pPhaseDirector_->SetEnemyManager(ctx_.enemyManager);
    pPhaseDirector_->SetPlayer(ctx_.player);
    pPhaseDirector_->SetWaveIndex(ctx_.waveIndex);
    pPhaseDirector_->ChangePhase(PhaseType::Battle);
}

void GeneralWave::BeginSetupPhase()
{
    if (!pPhaseDirector_)
    {
        return;
    }

    // ウェーブの終了は毎フレーム成立し続けるので、入り直さないようにする
    if (pPhaseDirector_->GetCurrentPhaseType() == PhaseType::Setup)
    {
        return;
    }

    pPhaseDirector_->ChangePhase(PhaseType::Setup);
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
    if (ctx_.enemyManager)
    {
        return ctx_.enemyManager->IsAllEnemiesDefeated();
    }
    return false;
}

bool GeneralWave::IsEnemyBudgetExhausted() const
{
    if (ctx_.enemyManager)
    {
        return ctx_.enemyManager->GetRemainingSpawnCount() == 0;
    }
    return false;
}
