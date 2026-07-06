#pragma once

#include "IPhase.h"
#include "PhaseType.h"
#include <memory>



class PhaseDirector
{
public:
    void Initialize();
    void Update();
    void Draw();
    void Finalize();

    void ChangePhase(PhaseType type);

    PhaseType GetCurrentPhaseType() const { return currentPhaseType_; }
    IPhase* GetCurrentPhase() const { return pCurrentPhase_.get(); }

private:
    std::unique_ptr<IPhase> CreatePhase(PhaseType type);

    PhaseType currentPhaseType_;
    std::unique_ptr<IPhase> pCurrentPhase_;
};