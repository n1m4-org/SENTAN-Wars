#include "SetupPhase.h"
#include <system/EventListener.h>
#include <events/WaveEvent.h>
#include <common/ResourcePath.h>
#include <utility/ViewportUnits.hpp>
#include <SpriteManager.h>


void SetupPhase::Enter()
{
    pWarpHole_ = std::make_unique<WarpHole>(Hagine::Vector3(0.0f, 1.0f, 0.0f));
    pWarpHole_->SetOnEnterCallback([this]
    {
        // ワープホールに入ったときの処理
        // ここで次ウェーブに移行するイベントを呼び出す
        EventListener::GetInstance()->Publish<Event::WarpConfirm>();
    });

    pSentanSelect_ = std::make_unique<SentanSelect>();

    // スプライトの初期化
    this->InitializeSprite();
}

void SetupPhase::Exit()
{
    Hagine::SpriteManager::GetInstance()->UnregisterExternal(pPrompt_.get());
}

void SetupPhase::Update()
{
    pWarpHole_->Update();

    pSentanSelect_->Update();

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

void SetupPhase::Draw()
{

}

void SetupPhase::InitializeSprite()
{
    pPrompt_ = std::make_unique<Hagine::Sprite>();
    pPrompt_->Initialize(Path::Image::NextWavePrompt, { 55_vw, 45_vh });
}
