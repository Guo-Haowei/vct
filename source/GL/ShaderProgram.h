#pragma once
#include "GpuResource.h"
#include "../math/GeoMath.h"

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

    void setUniform(const char* name, const mat4& mat);

protected:
    virtual void internalRelease() override;

    Handle compileAndAttachShader(const char* path, GLenum type);
}; 
