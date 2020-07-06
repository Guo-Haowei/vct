#include "GpuBuffer.h"

GpuBuffer::GpuBuffer(const std::string& debugName, const CreateInfo& info)
    : GpuResource(debugName)
    , m_type(info.type)
    , m_usage(info.usage)
{
    glCreateBuffers(1, &m_handle);
    // thoghts: use glBufferStorage if usage is GL_STATIC_DRAW?
    if (info.usage == GL_STATIC_DRAW)
    {
        glNamedBufferStorage(m_handle, info.initialBuffer.size, info.initialBuffer.data, 0);
    }
    else
    {
        glNamedBufferData(m_handle, info.initialBuffer.size, info.initialBuffer.data, m_usage);
    }
}

void GpuBuffer::internalRelease()
{
    glDeleteBuffers(1, &m_handle);
}


