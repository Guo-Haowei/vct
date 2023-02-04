#pragma once
#include <string>

class IManager {
public:
    IManager( const char* debugName )
        : m_debugName( debugName ), m_initialized( false )
    {
    }

    virtual ~IManager() = default;

    virtual bool Init() = 0;
    virtual void Deinit() = 0;

    const std::string& GetDebugName() const { return m_debugName; }
    bool IsInitialized() const { return m_initialized; }

protected:
    std::string m_debugName;
    bool m_initialized;
};

bool manager_init( IManager* manager );
void manager_deinit( IManager* manager );
