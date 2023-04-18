#include "ManagerBase.h"

#include "ManagerBase.h"
#include "Core/Check.h"

bool ManagerBase::Initialize()
{
    LOG_INFO("[Runtime] Initializing Manager '{}'...", mName);
    mInitialized = InitializeInternal();
    if (!mInitialized)
    {
        LOG_FATAL("[Runtime] Failed to initialize Manager '{}'", mName);
        return false;
    }

    LOG_INFO("[Runtime] Manager '{}' initialized", mName);
    return true;
}

void ManagerBase::Finalize()
{
    LOG_INFO("[Runtime] Finalizing Manager '{}'...", mName);
    FinalizeInternal();
    LOG_INFO("[Runtime] Manager '{}' finalized", mName);
}

void ManagerBase::Update(float)
{
}
