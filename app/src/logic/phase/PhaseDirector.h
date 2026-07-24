#pragma once

#include "IPhase.h"
#include "PhaseType.h"
#include "PhaseTransitionValidator.h"

#include <debug/GameParameter.h>
#include <memory>

// フェーズの管理を行うクラス
// フェーズの切り替えや、現在のフェーズの取得などを行う
// GeneralWaveやBossWaveなどのウェーブクラスが使用する

class EnemyManager;
class Player;

class PhaseDirector
{
public:
    void Initialize();
    void Update();
    void Draw();
    void Finalize();

    /// <summary>
    /// フェーズを切り替える
    /// </summary>
    /// <param name="type">フェーズ種類</param>
    void ChangePhase(PhaseType type);

    /// <summary>
    /// 現在のフェーズ種類を取得する
    /// </summary>
    PhaseType GetCurrentPhaseType() const { return currentPhaseType_; }

    /// <summary>
    /// 現在のフェーズのインスタンスを取得する
    /// </summary>
    void SetEnemyManager(EnemyManager* enemyManager) { pEnemyManager_ = enemyManager; }
    void SetWaveIndex(uint32_t waveIndex) { waveIndex_ = waveIndex; }

    /// 準備フェーズでSENTANを装備させる相手を渡す
    void SetPlayer(Player* player) { pPlayer_ = player; }

private:
    std::unique_ptr<IPhase> CreatePhase(PhaseType type);
    void RegisterOnChange();
    void RegisterCustomGuiFunctions();

    // 遷移の可否は現在のフェーズで決まるので、初期値を入れておく（Noneからはどこへでも入れる）
    PhaseType currentPhaseType_ = PhaseType::None;
    std::unique_ptr<IPhase> pCurrentPhase_;
    EnemyManager* pEnemyManager_ = nullptr;
    Player* pPlayer_ = nullptr;
    uint32_t waveIndex_ = 0;

    std::unique_ptr<PhaseTransitionValidator> pTransitionValidator_;

    EnableDebug("PhaseDirector");
    GameParameter(bool, goSetupPhase, false);
    GameParameter(bool, goBattlePhase, false);
    GameParameter(bool, goBossPhase, false);
};