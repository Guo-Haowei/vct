#pragma once
#include "Interface.hpp"

_Interface_ IRuntimeModule
{
public:
    IRuntimeModule() = default;
    virtual ~IRuntimeModule() = default;

    virtual bool Init() = 0;
    virtual void Deinit() = 0;

    virtual void Tick() = 0;
};
