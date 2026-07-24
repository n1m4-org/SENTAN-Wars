#pragma once
#include "IPhase.h"
#include <entity/WarpHole.h>
#include <Sprite.h>
#include <presentation/ui/SentanSelect.h>


class SetupPhase : public IPhase
{
public:
    void Enter() override;


    void Exit() override;


    void Update() override;


    void Draw() override;

private:
    void InitializeSprite();

    std::unique_ptr<WarpHole> pWarpHole_ = nullptr;
    std::unique_ptr<Hagine::Sprite> pPrompt_ = nullptr;
    std::unique_ptr<SentanSelect> pSentanSelect_ = nullptr;
};