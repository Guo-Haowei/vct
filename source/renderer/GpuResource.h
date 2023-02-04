#pragma once
#include "GLPrerequisites.h"

class GpuResource {
public:
    enum { NULL_HANDLE = 0u };
    inline GLuint getHandle() const { return m_handle; }

protected:
    GLuint m_handle = NULL_HANDLE;
};
