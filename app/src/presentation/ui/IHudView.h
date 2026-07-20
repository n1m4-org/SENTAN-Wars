#pragma once

class IHudView
{
public:
    virtual ~IHudView() = default;
    virtual void Update() = 0;
};