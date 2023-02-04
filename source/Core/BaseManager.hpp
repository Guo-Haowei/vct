#pragma once
#include <string>
#include "Interface/IRuntimeModule.hpp"

_Interface_ BaseManager : _Implements_ IRuntimeModule
{
public:
    BaseManager( const char* debugName )
        : m_debugName( debugName ), m_initialized( false )
    {
    }

    virtual ~BaseManager() = default;

    virtual bool Init() override { return true; }
    virtual void Deinit() override {}
    virtual void Tick() override {}

    const std::string& GetDebugName() const { return m_debugName; }
    bool IsInitialized() const { return m_initialized; }

protected:
    std::string m_debugName;
    bool m_initialized;
};

bool manager_init( BaseManager* manager );
void manager_deinit( BaseManager* manager );
