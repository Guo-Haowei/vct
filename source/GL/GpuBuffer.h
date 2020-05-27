#pragma once
#include "GpuResource.h"
#include "../Buffer.h"

class GpuBuffer : public GpuResource
{
public:
    struct CreateInfo
    {
        GLenum type; // GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_UNIFORM_BUFFER
        GLenum usage; // GL_STATIC_DRAW, GL_STREAM_DRAW
        Buffer initialBuffer;
    };

    GpuBuffer(const std::string& debugName, const CreateInfo& info);

    GLenum getType() const { return m_type; }
    GLenum getUsage() const { return m_usage; }
protected:
    virtual void internalRelease() override;

    GLenum m_type;
    GLenum m_usage;
}; 

