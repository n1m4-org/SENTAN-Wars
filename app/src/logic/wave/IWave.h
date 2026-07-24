#pragma once
#include <cstdint>

class EnemyManager;
class Player;

struct WaveContext
{
    // ウェーブのインデックス (UIで表示するときに使える)
    uint32_t waveIndex = 0;
    EnemyManager* enemyManager = nullptr;
    // 準備フェーズでSENTANを装備させる相手
    Player* player = nullptr;
};

class IWave
{
public:
    virtual ~IWave() = default;
    virtual void Enter(const WaveContext& ctx) = 0;
    virtual void Exit() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;

    /// 準備フェーズ（ポータルとSENTAN選択）に入る
    /// ウェーブ終了後、次のウェーブへ進む前の待ち時間として使う
    /// すでに準備フェーズなら何もしない（何度呼んでも入り直さない）
    virtual void BeginSetupPhase() = 0;

    /// ウェーブが終了したかどうか (trueの場合、次のウェーブに移行できる)
    virtual bool IsWaveFinished() const = 0;
    /// 予算が付きたかどうか
    virtual bool IsEnemyBudgetExhausted() const = 0;
};