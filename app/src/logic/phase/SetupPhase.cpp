#include "SetupPhase.h"



void SetupPhase::Enter()
{
    pWarpHole_ = std::make_unique<WarpHole>(Hagine::Vector3(0.0f, 1.0f, 0.0f));
}
