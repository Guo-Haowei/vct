#pragma once
#include "GpuResource.h"
#include "universal/core_math.h"

namespace vct {

class GlslProgram : public GpuResource {
   public:
    void createFromFiles( const char* vert, const char* frag, const char* geom = nullptr );
    void createFromFile( const char* comp );
    void use();
    void stop();
    void destroy();
    GLint getUniformLocation( const char* name );
    void setUniform( GLint location, const int& val );
    void setUniform( GLint location, const float& val );
    void setUniform( GLint location, const vec2& val );
    void setUniform( GLint location, const vec3& val );
    void setUniform( GLint location, const vec4& val );
    void setUniform( GLint location, const mat4& val );

   private:
    void linkProgram( const char* file );
};

}  // namespace vct
