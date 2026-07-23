#pragma once

#include "IPhase.h"
#include "PhaseType.h"
#include "PhaseTransitionValidator.h"

#include <debug/GameParameter.h>
#include <memory>

// フェーズの管理を行うクラス
// フェーズの切り替えや、現在のフェーズの取得などを行う
// GeneralWaveやBossWaveなどのウェーブクラスが使用する

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
    IPhase* GetCurrentPhase() const { return pCurrentPhase_.get(); }

private:
    std::unique_ptr<IPhase> CreatePhase(PhaseType type);
    void RegisterOnChange();
    void RegisterCustomGuiFunctions();

    PhaseType currentPhaseType_;
    std::unique_ptr<IPhase> pCurrentPhase_;

    std::unique_ptr<PhaseTransitionValidator> pTransitionValidator_;

    EnableDebug("PhaseDirector");
    GameParameter(bool, goSetupPhase, false);
    GameParameter(bool, goBattlePhase, false);
    GameParameter(bool, goBossPhase, false);
};