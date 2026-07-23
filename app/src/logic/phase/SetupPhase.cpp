#include "SetupPhase.h"
#include <system/EventListener.h>
#include <events/WaveEvent.h>

void SetupPhase::Enter()
{
    pWarpHole_ = std::make_unique<WarpHole>(Hagine::Vector3(0.0f, 1.0f, 0.0f));
    pWarpHole_->SetOnEnterCallback([this]
    {
        // ワープホールに入ったときの処理
        // ここで次ウェーブに移行するイベントを呼び出す
        EventListener::GetInstance()->Publish<Event::WarpConfirm>();
    });


}

void SetupPhase::Exit()
{

}

void SetupPhase::Update()
{

}

void SetupPhase::Draw()
{

}
