#pragma once
#include "IRuntimeModule.hpp"

class IGameLogic : public IRuntimeModule {
public:
    virtual bool Initialize() = 0;
    virtual void Finalize() = 0;

    virtual void Tick() = 0;
};