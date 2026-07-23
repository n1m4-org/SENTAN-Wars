#pragma once
#include "IWave.h"
#include "WaveType.h"
#include <debug/GameParameter.h>
#include <memory>
#include <system/EventSubscription.h>


/// <summary>
/// ウェーブの保持し、ウェーブの切り替えを行うクラス
/// ウェーブにアクセスする場合はこのクラスから取得する
/// </summary>
class WaveDirector
{
public:
    void Initialize();
    void Update();

    /// <summary>
    /// ウェーブをインデックス指定で切り替え
    /// </summary>
    void ChangeWaveByIndex(uint32_t index);

    /// <summary>
    /// 次のウェーブに切り替え
    /// </summary>
    void ChangeToNextWave() { this->ChangeWaveByIndex(waveIndex_ + 1); }

    /// <summary>
    /// ウェーブの種類を取得する
    /// </summary>
    /// <param name="waveIndex">ウェーブのインデックス (注：0<=)</param>
    /// <returns></returns>
    WaveType GetWaveType(uint32_t waveIndex) const;

    /// <summary>
    /// 現在のウェーブを取得する
    /// </summary>
    /// <returns>ウェーブ基底クラス</returns>
    IWave* GetCurrentWave() const { return pCurrentWave_.get(); }

    /// <summary>
    /// 現在のウェーブのインデックスを取得する
    /// </summary>
    /// <returns></returns>
    uint32_t GetCurrentWaveIndex() const { return waveIndex_; }

    /// <summary>
    /// EnemyManagerを設定する
    /// </summary>
    /// <param name="enemyManager"></param>
    void SetEnemyManager(EnemyManager* enemyManager) { pEnemyManager_ = enemyManager; }

private:
    std::unique_ptr<IWave> CreateWave(WaveType type);
    void RegisterOnChange();

    EnemyManager* pEnemyManager_ = nullptr;

    // 現在のウェーブ
    std::unique_ptr<IWave> pCurrentWave_;

    // ワープホールの確認イベントの購読ハンドル
    EventSubscription subWarpConfirm_;

    /// パラメータ
    EnableDebug("WaveDirector");
    GameParameter(int, waveIndex_, 0);
    GameParameter(bool, goNextWave_, false);
    GameParameter(int, numGeneralWavePerLap_, 3);
};