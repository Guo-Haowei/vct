#include "BaseManager.hpp"

#include "Base/Asserts.h"
#include "Base/Logger.h"

bool manager_init( BaseManager* manager )
{
    ASSERT( manager && !manager->IsInitialized() );
    const char* debugName = manager->GetDebugName().c_str();
    LOG_INFO( "manager_init: Initializing manager '%s'...", debugName );
    if ( !manager->Initialize() ) {
        LOG_ERROR( "manager_init: Failed to initialize manager '%s'...", debugName );
        return false;
    }
    LOG_OK( "manager_init: Manager '%s' initialized.\n", debugName );
    return true;
}

void manager_deinit( BaseManager* manager )
{
    ASSERT( manager && manager->IsInitialized() );
    LOG_INFO( "manager_deinit: Manager '%s' finalized", manager->GetDebugName().c_str() );
}