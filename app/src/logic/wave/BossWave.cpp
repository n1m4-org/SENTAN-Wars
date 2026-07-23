#include "BossWave.h"
#include "Character/Enemy/EnemyManager.h"
#include "logic/phase/BossPhase.h"

void BossWave::Enter(const WaveContext& ctx)
{
    ctx_ = ctx;
    pPhaseDirector_ = std::make_unique<PhaseDirector>();
    pPhaseDirector_->Initialize();
    pPhaseDirector_->SetEnemyManager(ctx_.enemyManager);
    pPhaseDirector_->SetWaveIndex(ctx_.waveIndex);
    pPhaseDirector_->ChangePhase(PhaseType::Boss);
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

bool BossWave::IsWaveFinished() const
{
    if (ctx_.enemyManager)
    {
        return ctx_.enemyManager->IsAllEnemiesDefeated();
    }
    return false;
}

bool BossWave::IsEnemyBudgetExhausted() const
{
    if (ctx_.enemyManager)
    {
        return ctx_.enemyManager->GetRemainingSpawnCount() == 0;
    }
    return false;
}
