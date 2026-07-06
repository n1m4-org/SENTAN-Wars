#include "PhaseDirector.h"
#include <cassert>



void PhaseDirector::Initialize()
{
    
}

void PhaseDirector::Update()
{

}

void PhaseDirector::Draw()
{

}

void PhaseDirector::Finalize()
{

}

void PhaseDirector::ChangePhase(PhaseType type)
{
    currentPhaseType_ = type;
    pCurrentPhase_ = CreatePhase(type);
}

std::unique_ptr<IPhase> PhaseDirector::CreatePhase(PhaseType type)
{
    switch (type)
    {
    case PhaseType::Setup:
        return nullptr;
        break;
    case PhaseType::Buttle:
        return nullptr;
        break;
    case PhaseType::Boss:
        return nullptr;
        break;
    default:
        assert(false && "Invalid PhaseType");
    }

    return nullptr;
}
