#pragma once
#include "IPhase.h"
#include "Character/Enemy/EnemyManager.h"
#include <debugapi.h>

class BattlePhase : public IPhase
{
public:
    explicit BattlePhase(EnemyManager* enemyManager = nullptr)
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
        OutputDebugStringA("BattlePhase::Enter\n");
        if (pEnemyManager_)
        {
            const WaveData& waveData = pEnemyManager_->GetWavePreset(waveIndex_);
            pEnemyManager_->StartWave(waveData);
        }
    }
    void Exit() override
    {
        OutputDebugStringA("BattlePhase::Exit\n");
    }
    void Update() override
    {
        OutputDebugStringA("BattlePhase::Update\n");
    }
    void Draw() override
    {
        OutputDebugStringA("BattlePhase::Draw\n");
    }

private:
    EnemyManager* pEnemyManager_ = nullptr;
    uint32_t waveIndex_ = 0;
};