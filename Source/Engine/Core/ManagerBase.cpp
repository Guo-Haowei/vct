#include "ManagerBase.h"

#include "ManagerBase.h"
#include "Core/Check.h"

#define DEBUG_MANAGER_BASE(FMT, ...) LOG_DEBUG(__FUNCTION__ " " FMT, ##__VA_ARGS__)

bool ManagerBase::Initialize()
{
    DEBUG_MANAGER_BASE("Initializing Manager '{}'...", mName);
    mInitialized = InitializeInternal();
    if (!mInitialized)
    {
        LOG_FATAL(__FUNCTION__ " Failed to initialize Manager '{}'", mName);
        return false;
    }

    DEBUG_MANAGER_BASE("Manager '{}' initialized", mName);
    return true;
}

void ManagerBase::Finalize()
{
    DEBUG_MANAGER_BASE("Finalizing Manager '{}'...", mName);
    FinalizeInternal();
    DEBUG_MANAGER_BASE("Manager '{}' finalized", mName);
}

void ManagerBase::Update(float)
{
}
