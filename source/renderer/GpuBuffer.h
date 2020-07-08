#pragma once
#include "GpuResource.h"

namespace vct {

template<typename T>
class UniformBuffer : public GpuResource
{
public:
    void destroy()
    {
        if (m_handle != NULL_HANDLE)
            glDeleteBuffers(1, &m_handle);
        m_handle = NULL_HANDLE;
    }

    void createAndBind(int slot)
    {
        glGenBuffers(1, &m_handle);
        glBindBuffer(GL_UNIFORM_BUFFER, m_handle);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(T), 0, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, slot, m_handle);
    }

    void update()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_handle);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &cache, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    T cache;
};

} // namespace vct
