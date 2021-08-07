#pragma once
#include "GpuResource.h"
#include "universal/core_math.h"

class GlslProgram {
   public:
    void Create( GLuint program );
    void Destroy();

    void Use() const;
    void Stop();

    GLint GetUniformLocation( const char* name ) const;
    void SetUniform( GLint location, const int& val ) const;
    void SetUniform( GLint location, const float& val ) const;
    void SetUniform( GLint location, const vec2& val ) const;
    void SetUniform( GLint location, const vec3& val ) const;
    void SetUniform( GLint location, const vec4& val ) const;
    void SetUniform( GLint location, const mat4& val ) const;

   private:
    // GlslProgram( const GlslProgram& ) = delete;

    GLuint mHandle = 0;
};
