#pragma once
#include "GlslProgram.h"

#include "universal/core_assert.h"
#include "universal/print.h"

void GlslProgram::Create( GLuint program )
{
    mHandle = program;
}

void GlslProgram::Destroy()
{
    if ( mHandle != 0 )
    {
        glDeleteProgram( mHandle );
    }

    mHandle = 0;
}

void GlslProgram::Use() const
{
    glUseProgram( mHandle );
}

void GlslProgram::Stop()
{
    glUseProgram( 0 );
}

GLint GlslProgram::GetUniformLocation( const char* name ) const
{
    GLint location = glGetUniformLocation( mHandle, name );
    if ( location < 0 )
    {
        Com_PrintWarning( "uniform '%s' not found" );
    }
    return location;
}

void GlslProgram::SetUniform( GLint location, const int& val ) const
{
    glUniform1i( location, val );
}

void GlslProgram::SetUniform( GLint location, const float& val ) const
{
    glUniform1f( location, val );
}

void GlslProgram::SetUniform( GLint location, const vec2& val ) const
{
    glUniform2f( location, val.x, val.y );
}

void GlslProgram::SetUniform( GLint location, const vec3& val ) const
{
    glUniform3f( location, val.x, val.y, val.z );
}

void GlslProgram::SetUniform( GLint location, const vec4& val ) const
{
    glUniform4f( location, val.x, val.y, val.z, val.w );
}

void GlslProgram::SetUniform( GLint location, const mat4& val ) const
{
    glUniformMatrix4fv( location, 1, GL_FALSE, &val[0].x );
}
