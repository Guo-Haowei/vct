#pragma once
#include <glad/glad.h>
#include <string>

class GpuResource
{
    GpuResource(const GpuResource&) = delete;
    GpuResource& operator=(const GpuResource&) = delete;
public:
    using Handle = GLuint;
    enum { INVALID_HANDLE = 0 };
public:
    GpuResource(const std::string& debugName);
    virtual ~GpuResource();
    void release();
    GLuint getHandle() const { return m_handle; }
protected:
    virtual void internalRelease() = 0;
protected:
    Handle m_handle;
    std::string m_debugName;
};
