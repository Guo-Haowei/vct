#include "GpuResource.h"
#include <assert.h>

GpuResource::GpuResource(const std::string& debugName)
    : m_handle(INVALID_HANDLE), m_debugName(debugName)
{
}

GpuResource::~GpuResource()
{
    // make sure gpu resource is properly released
    assert(m_handle == 0);
}


void GpuResource::release()
{
    if (m_handle != INVALID_HANDLE)
    {
        internalRelease(); 
        m_handle = 0;
    }
}
