#pragma once

class PhaseTransitionPresenter
{
public:
    void Update();
    void PhaseChanged();

private:
    bool isPhaseChanged_ = false;
};