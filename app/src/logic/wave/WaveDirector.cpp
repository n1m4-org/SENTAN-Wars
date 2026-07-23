#include "WaveDirector.h"
#include "GeneralWave.h"
#include "BossWave.h"
#include <events/WaveEvent.h>
#include <system/EventListener.h>



void WaveDirector::Initialize()
{
    // デバッグ用のパラメータの変更コールバックを登録
    this->RegisterOnChange();

    // 初期ウェーブを作成
    this->ChangeWaveByIndex(0);

    // ワープホールの確認イベントを購読
    subWarpConfirm_ = EventListener::GetInstance()->Subscribe<Event::WarpConfirm>([this](const Event::WarpConfirm&)
    {
        // ワープホールの確認イベントが発生したら、次のウェーブに切り替える
        this->ChangeToNextWave();
    });
}

void WaveDirector::Update()
{
    if (pCurrentWave_)
    {
        pCurrentWave_->Update();

        // ウェーブが終了（敵が全滅等）したら次のウェーブへ
        if (pCurrentWave_->IsWaveFinished())
        {
            this->ChangeToNextWave();
        }
    }
}

WaveType WaveDirector::GetWaveType(uint32_t waveIndex) const
{
    const uint32_t kBossWaveIndexPerLap = numGeneralWavePerLap_;
    if (waveIndex % kBossWaveIndexPerLap == 0 && waveIndex != 0)
    {
        return WaveType::Boss;
    }
    return WaveType::General;
}

std::unique_ptr<IWave> WaveDirector::CreateWave(WaveType type)
{
    switch (type)
    {
    case WaveType::General:
        return std::make_unique<GeneralWave>();
        break;
    case WaveType::Boss:
        return std::make_unique<BossWave>();
        break;
    default:
        return nullptr;
        break;
    }
}

void WaveDirector::RegisterOnChange()
{
    #ifdef _DEBUG
    debug_goNextWave_.SetOnChange([this](const bool& value)
    {
        if (value)
        {
            this->ChangeWaveByIndex(waveIndex_ + 1);
            goNextWave_ = false;
        }
    });
    #endif // _DEBUG
}

void WaveDirector::ChangeWaveByIndex(uint32_t index)
{
    if (pCurrentWave_)
    {
        pCurrentWave_->Exit();
    }

    waveIndex_ = index;
    WaveType nextWaveType = this->GetWaveType(index);
    pCurrentWave_ = this->CreateWave(nextWaveType);

    WaveContext ctx;
    ctx.waveIndex = index;
    ctx.enemyManager = pEnemyManager_;
    pCurrentWave_->Enter(ctx);
}
