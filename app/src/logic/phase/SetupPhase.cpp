#include "SetupPhase.h"
#include <system/EventListener.h>
#include <events/WaveEvent.h>
#include <common/ResourcePath.h>
#include <utility/ViewportUnits.hpp>
#include <SpriteManager.h>
#include <Character/Player/Player.h>
#include <logic/EquippedSentanCache.h>


void SetupPhase::Enter()
{
    pWarpHole_ = std::make_unique<WarpHole>(Hagine::Vector3(0.0f, 1.0f, 0.0f));
    pWarpHole_->SetOnEnterCallback([this]
    {
        // ワープホールに入ったときの処理
        // ここで次ウェーブに移行するイベントを呼び出す
        EventListener::GetInstance()->Publish<Event::WarpConfirm>();
    });

    if (pPlayer_)
    {
        // 準備フェーズのたびに装備を丸ごと選び直すので、まず今の装備を外す
        // 外したSENTANの振る舞いはコンポーネントごと消えるため、前の攻撃は残らない
        pPlayer_->UnequipAllSentan();
        EquippedSentanCache::GetInstance()->Reset();

        // 上限のぶんだけ選ばせる
        pSentanSelect_ = std::make_unique<SentanSelect>(static_cast<uint32_t>(Player::GetMaxSentanCount()));
        pSentanSelect_->SetOnDecideCallback([this](SentanId id) { this->EquipSelectedSentan(id); });
    }

    // スプライトの初期化
    this->InitializeSprite();
}

void SetupPhase::Exit()
{
    Hagine::SpriteManager::GetInstance()->UnregisterExternal(pPrompt_.get());
}

void SetupPhase::Update()
{
    // SENTANを選び終わるまではポータルへ進めない（選ばずに次のウェーブへ行けてしまわないようにする）
    if (pSentanSelect_ && !pSentanSelect_->IsClosed())
    {
        pSentanSelect_->Update();

        if (pSentanSelect_->IsDecided())
        {
            // 表示を消すだけで、実体はこのフェーズが終わるまで残す
            // 選び終えた場で壊すと、直前のフレームの描画コマンドが解放済みのスプライトを読みに行く
            pSentanSelect_->Close();
        }
        return;
    }

    pWarpHole_->Update();

    auto sm = Hagine::SpriteManager::GetInstance();

    if (pWarpHole_->IsPlayerInside())
    {
        sm->RegisterExternal(pPrompt_.get());
    }
    else
    {
        sm->UnregisterExternal(pPrompt_.get());
    }

}

void SetupPhase::EquipSelectedSentan(SentanId id)
{
    if (!pPlayer_)
    {
        return;
    }

    // 装備できたときだけ「持っている」ことにする
    // 上限に達していると付かないので、持っていない物が選択肢から外れてしまわないようにする
    if (pPlayer_->EquipSentan(id))
    {
        EquippedSentanCache::GetInstance()->RecordEquippedSentan(id);
    }
}

void SetupPhase::Draw()
{

}

void SetupPhase::InitializeSprite()
{
    pPrompt_ = std::make_unique<Hagine::Sprite>();
    pPrompt_->Initialize(Path::Image::NextWavePrompt, { 55_vw, 45_vh });
}
