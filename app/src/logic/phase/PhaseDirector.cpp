#include "PhaseDirector.h"
#include <cassert>

#include "SetupPhase.h"
#include "BattlePhase.h"
#include "BossPhase.h"


void PhaseDirector::Initialize()
{
    this->RegisterOnChange();
    this->RegisterCustomGuiFunctions();

    pTransitionValidator_ = std::make_unique<PhaseTransitionValidator>(currentPhaseType_);
}

void PhaseDirector::Update()
{
    pCurrentPhase_->Update();
}

void PhaseDirector::Draw()
{
    pCurrentPhase_->Draw();
}

void PhaseDirector::Finalize()
{
    pCurrentPhase_->Exit();
    pCurrentPhase_.reset();
}

void PhaseDirector::ChangePhase(PhaseType type)
{
    if (!pTransitionValidator_->Validate(type))
    {
        assert(false && "Invalid phase transition");
        return;
    }

    // 現在のフェーズを終了
    if (pCurrentPhase_)
    {
        pCurrentPhase_->Exit();
    }

    currentPhaseType_ = type;
    pCurrentPhase_ = CreatePhase(type);
    pCurrentPhase_->Enter();
}

std::unique_ptr<IPhase> PhaseDirector::CreatePhase(PhaseType type)
{
    switch (type)
    {
    case PhaseType::Setup:
        return std::make_unique<SetupPhase>();
        break;
    case PhaseType::Battle:
        return std::make_unique<BattlePhase>();
        break;
    case PhaseType::Boss:
        return std::make_unique<BossPhase>();
        break;
    default:
        assert(false && "Invalid PhaseType");
    }

    return nullptr;
}

void PhaseDirector::RegisterOnChange()
{
#ifdef _DEBUG

    debug_goSetupPhase.SetOnChange([this](const bool& value)
    {
        if (value)
        {
            this->ChangePhase(PhaseType::Setup);
            goSetupPhase = false;
        }
    });

    debug_goBattlePhase.SetOnChange([this](const bool& value)
    {
        if (value)
        {
            this->ChangePhase(PhaseType::Battle);
            goBattlePhase = false;
        }
    });

    debug_goBossPhase.SetOnChange([this](const bool& value)
    {
        if (value)
        {
            this->ChangePhase(PhaseType::Boss);
            goBossPhase = false;
        }
    });

#endif // _DEBUG
}

void PhaseDirector::RegisterCustomGuiFunctions()
{
#ifdef _DEBUG

    auto funcTransitionValidator = [this]()
    {
        bool bypass = pTransitionValidator_->IsBypass();
        ImGui::Checkbox("Bypass", &bypass);
        pTransitionValidator_->Bypass(bypass);
    };
    debugEntry.RegisterCustomGuiFunction("Transition Validator", funcTransitionValidator);

#endif // _DEBUG
}
