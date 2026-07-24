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

    int GetTargetBossDefeatCount() const { return targetBossDefeatCount_; }
    void SetTargetBossDefeatCount(int count) { targetBossDefeatCount_ = count; }
    int GetBossDefeatCount() const { return bossDefeatCount_; }
    bool IsGameCleared() const { return isGameCleared_; }

    /// <summary>
    /// EnemyManagerを設定する
    /// </summary>
    /// <param name="enemyManager"></param>
    void SetEnemyManager(EnemyManager* enemyManager) { pEnemyManager_ = enemyManager; }

    /// <summary>
    /// 準備フェーズでSENTANを装備させる相手を設定する
    /// </summary>
    /// <remarks>最初のウェーブにも渡すため、Initializeより前に呼ぶ</remarks>
    void SetPlayer(Player* player) { pPlayer_ = player; }

private:
    std::unique_ptr<IWave> CreateWave(WaveType type);
    void RegisterOnChange();

    EnemyManager* pEnemyManager_ = nullptr;
    Player* pPlayer_ = nullptr;
    bool isGameCleared_ = false;
    int bossDefeatCount_ = 0;

    // このウェーブの終了処理をもう通したか
    // 準備フェーズで待つ間もウェーブの終了は成立し続けるため、1回だけ通すのに使う
    bool isWaveFinishHandled_ = false;

    // 現在のウェーブ
    std::unique_ptr<IWave> pCurrentWave_;

    // ワープホールの確認イベントの購読ハンドル
    EventSubscription subWarpConfirm_;

    /// パラメータ
    EnableDebug("WaveDirector");
    GameParameter(int, waveIndex_, 0);
    GameParameter(bool, goNextWave_, false);
    GameParameter(int, numGeneralWavePerLap_, 3);
	GameParameter(int, targetBossDefeatCount_, 3); // 目標のボス討伐数
};