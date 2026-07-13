#pragma once
#include "PhaseType.h"

class PhaseTransitionValidator
{
public:
    PhaseTransitionValidator(PhaseType& type) : currentPhaseTypeRef_(type) {}
    inline bool Validate(PhaseType newPhase)
    {
        // デバッグ用のバイパスフラグが有効な場合は、すべてのフェーズ遷移を許可する
        if (bypass_) return true;

        // 初期状態からの遷移はすべて許可する
        if (currentPhaseTypeRef_ == PhaseType::None)
            return true;

        // Battle -> Boss は禁止
        if (currentPhaseTypeRef_ == PhaseType::Battle && newPhase == PhaseType::Boss)
            return false;

        // Boss -> Battle は禁止
        if (currentPhaseTypeRef_ == PhaseType::Boss && newPhase == PhaseType::Battle)
            return false;

        // Battle -> Setup を許可
        if (currentPhaseTypeRef_ == PhaseType::Battle && newPhase == PhaseType::Setup)
            return true;
        
        // Boss -> Setup を許可
        if (currentPhaseTypeRef_ == PhaseType::Boss && newPhase == PhaseType::Setup)
            return true;

        return false;
    }

    // デバッグ用のバイパスフラグ
    void Bypass(bool v) { bypass_ = v; }
    bool IsBypass() const { return bypass_; }

private:
    PhaseType& currentPhaseTypeRef_;
    bool bypass_ = false;
};