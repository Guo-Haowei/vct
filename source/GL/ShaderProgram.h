#pragma once
#include "GpuResource.h"

class ShaderProgram : public GpuResource
{
public:
    struct CreateInfo
    {
        const char* vs = nullptr; /// vertex shader file path
        const char* gs = nullptr; /// geometry shader file path
        const char* fs = nullptr; /// fragment shader file path
    };

    ShaderProgram(const std::string& debugName, const CreateInfo& info);
protected:
    virtual void internalRelease() override;

    Handle compileAndAttachShader(const char* path, GLenum type);
}; 
