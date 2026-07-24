#pragma once
#include "IPhase.h"
#include <entity/WarpHole.h>
#include <Sprite.h>
#include <presentation/ui/SentanSelect.h>


class Player;

class SetupPhase : public IPhase
{
public:
    /// SENTANを装備させる相手を受け取る（渡されなければ選択も装備もしない）
    explicit SetupPhase(Player* player = nullptr) : pPlayer_(player) {}

    void Enter() override;


    void Exit() override;


    void Update() override;


    void Draw() override;

private:
    void InitializeSprite();

    /// 選ばれたSENTANを装備させる
    void EquipSelectedSentan(SentanId id);

    // SENTANを装備させる相手（所有はしない）
    Player* pPlayer_ = nullptr;

    std::unique_ptr<WarpHole> pWarpHole_ = nullptr;
    std::unique_ptr<Hagine::Sprite> pPrompt_ = nullptr;
    std::unique_ptr<SentanSelect> pSentanSelect_ = nullptr;
};