#pragma once
#include <cstdint>

struct WaveContext
{

};

class IWave
{
public:
    virtual ~IWave() = default;
    virtual void Enter(const WaveContext& ctx) = 0;
    virtual void Exit() = 0;
    virtual void Update() = 0;

    /// ウェーブのインデックス (カウントなどに使える)
    virtual uint32_t GetWaveIndex() const = 0;
    /// ウェーブが終了したかどうか (trueの場合、次のウェーブに移行できる)
    virtual bool IsWaveFinished() const = 0;
    /// 予算が付きたかどうか
    virtual bool IsEnemyBudgetExhausted() const = 0;
};