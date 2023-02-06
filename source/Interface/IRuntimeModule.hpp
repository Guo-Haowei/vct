#pragma once

class IRuntimeModule
{
public:
    IRuntimeModule() = default;
    virtual ~IRuntimeModule() = default;

    virtual bool Initialize() = 0;
    virtual void Finalize() = 0;

    virtual void Tick() = 0;
};
