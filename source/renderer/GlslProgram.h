#pragma once
#include "GpuResource.h"
#include "base/GeoMath.h"

namespace vct {

class GlslProgram : public GpuResource
{
public:
    void createFromFiles(const char* vert, const char* frag, const char* geom = nullptr);
    void use();
    void stop();
    void destroy();
    GLint getUniformLocation(const char* name);
    void setUniform(GLint location, const int& val);
    void setUniform(GLint location, const float& val);
    void setUniform(GLint location, const Vector2& val);
    void setUniform(GLint location, const Vector3& val);
    void setUniform(GLint location, const Vector4& val);
    void setUniform(GLint location, const Matrix4& val);
};

} // namespace vct
