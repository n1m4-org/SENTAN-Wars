#pragma once

class PhaseChangePresentation
{
public:
    void Update();
    void PhaseChanged();

private:
    bool isPhaseChanged_ = false;
};