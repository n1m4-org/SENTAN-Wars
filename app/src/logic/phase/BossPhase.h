#pragma once

#include "IPhase.h"
#include "Character/Enemy/EnemyManager.h"
#include <debugapi.h>

class BossPhase : public IPhase
{
public:
    explicit BossPhase(EnemyManager* enemyManager = nullptr)
        : pEnemyManager_(enemyManager)
    {}

    void SetEnemyManager(EnemyManager* enemyManager)
    {
        pEnemyManager_ = enemyManager;
    }

    void SetWaveIndex(uint32_t waveIndex)
    {
        waveIndex_ = waveIndex;
    }

    void Enter() override
    {
        OutputDebugStringA("BossPhase::Enter\n");
        if (pEnemyManager_)
        {
            const WaveData& waveData = pEnemyManager_->GetWavePreset(waveIndex_);
            pEnemyManager_->StartWave(waveData);
        }
    }

    void Exit() override
    {
        OutputDebugStringA("BossPhase::Exit\n");
    }

    void Update() override
    {
        OutputDebugStringA("BossPhase::Update\n");
    }

    void Draw() override
    {
        OutputDebugStringA("BossPhase::Draw\n");
    }

private:
    EnemyManager* pEnemyManager_ = nullptr;
    uint32_t waveIndex_ = 0;
};