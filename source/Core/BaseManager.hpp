#pragma once
#include <string>
#include "Interface/IRuntimeModule.hpp"

class BaseManager : public IRuntimeModule
{
public:
    BaseManager( const char* debugName )
        : m_debugName( debugName ), m_initialized( false )
    {
    }

    virtual ~BaseManager() = default;

    virtual bool Initialize() override { return true; }
    virtual void Finalize() override {}
    virtual void Tick() override {}

    const std::string& GetDebugName() const { return m_debugName; }
    bool IsInitialized() const { return m_initialized; }

protected:
    std::string m_debugName;
    bool m_initialized;
};

bool manager_init( BaseManager* manager );
void manager_deinit( BaseManager* manager );
