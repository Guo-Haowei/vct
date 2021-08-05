#pragma once
#include "GlslProgram.h"

#include "universal/core_assert.h"
#include "universal/print.h"

namespace vct {

void GlslProgram::destroy()
{
    if ( m_handle != NULL_HANDLE )
        glDeleteProgram( m_handle );
    m_handle = NULL_HANDLE;
}

void GlslProgram::use()
{
    glUseProgram( m_handle );
}

void GlslProgram::stop()
{
    glUseProgram( NULL_HANDLE );
}

GLint GlslProgram::getUniformLocation( const char* name )
{
    GLint location = glGetUniformLocation( m_handle, name );
    if ( location < 0 )
    {
        Com_PrintWarning( "uniform '%s' not found" );
    }
    return location;
}

void GlslProgram::setUniform( GLint location, const int& val )
{
    glUniform1i( location, val );
}

void GlslProgram::setUniform( GLint location, const float& val )
{
    glUniform1f( location, val );
}

void GlslProgram::setUniform( GLint location, const vec2& val )
{
    glUniform2f( location, val.x, val.y );
}

void GlslProgram::setUniform( GLint location, const vec3& val )
{
    glUniform3f( location, val.x, val.y, val.z );
}

void GlslProgram::setUniform( GLint location, const vec4& val )
{
    glUniform4f( location, val.x, val.y, val.z, val.w );
}

void GlslProgram::setUniform( GLint location, const mat4& val )
{
    glUniformMatrix4fv( location, 1, GL_FALSE, &val[0].x );
}

}  // namespace vct
