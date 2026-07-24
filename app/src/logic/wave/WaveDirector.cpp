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
    // 破棄待ちの後始末は、現在のウェーブが無くても進める必要がある
    this->UpdatePendingDeleteWaves();

    if (!pCurrentWave_)
    {
        return;
    }

    pCurrentWave_->Update();

    // ウェーブが終了（敵が全滅等）していなければ何もしない
    if (!pCurrentWave_->IsWaveFinished())
    {
        return;
    }

    // 準備フェーズで待っている間もウェーブの終了は成立し続けるので、終わった瞬間だけ通す
    if (isWaveFinishHandled_)
    {
        return;
    }
    isWaveFinishHandled_ = true;

    // ボスウェーブが完了した場合、ボス討伐カウントをインクリメント
    if (this->GetWaveType(waveIndex_) == WaveType::Boss)
    {
        bossDefeatCount_++;

        // 目標のボス討伐数に達した場合、ゲームクリア
        if (targetBossDefeatCount_ > 0 && bossDefeatCount_ >= targetBossDefeatCount_)
        {
            isGameCleared_ = true;
            this->RetireCurrentWave();
            return;
        }
    }

    // 次のウェーブへは自動で進まず、準備フェーズ（ポータルとSENTAN選択）に入る
    // ポータルに入るとWarpConfirmが飛び、Initializeで登録した購読側が次のウェーブへ切り替える
    pCurrentWave_->BeginSetupPhase();
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

void WaveDirector::RetireCurrentWave()
{
    if (!pCurrentWave_)
    {
        return;
    }

    // 先に表示や登録を落としてから、破棄は後回しにする
    pCurrentWave_->Exit();
    pendingDeleteWaves_.emplace_back(PendingDeleteWave{std::move(pCurrentWave_), kWaveDeleteDelayFrames});
    pCurrentWave_.reset();
}

void WaveDirector::UpdatePendingDeleteWaves()
{
    std::erase_if(pendingDeleteWaves_, [](PendingDeleteWave &pending)
    {
        pending.delayFrames--;
        return pending.delayFrames <= 0;
    });
}

void WaveDirector::ChangeWaveByIndex(uint32_t index)
{
    // 前のウェーブはここでは壊さない
    // フェーズが持っているスプライトなどを、GPUがまだ前フレームの描画で使っている
    this->RetireCurrentWave();

    waveIndex_ = index;

    // 新しいウェーブはまだ終わっていないので、終了時の処理を受け付け直す
    isWaveFinishHandled_ = false;

    WaveType nextWaveType = this->GetWaveType(index);
    pCurrentWave_ = this->CreateWave(nextWaveType);

    WaveContext ctx;
    ctx.waveIndex = index;
    ctx.enemyManager = pEnemyManager_;
    ctx.player = pPlayer_;
    pCurrentWave_->Enter(ctx);
}
