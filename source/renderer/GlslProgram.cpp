#pragma once
#include "GlslProgram.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <vector>

#include "universal/core_assert.h"
#include "universal/print.h"

using std::ifstream;
using std::istreambuf_iterator;
using std::string;
using std::stringstream;
using std::vector;

namespace vct {

string readAsciiFile( const char* path )
{
    ifstream file( path );
    if ( !file.is_open() )
    {
        Com_PrintError( "failed to open file '%s'", path );
    }
    return string( ( istreambuf_iterator<char>( file ) ), istreambuf_iterator<char>() );
}

static GLuint createShaderFromFile( const char* file, GLenum shaderType );

void GlslProgram::linkProgram( const char* file )
{
    // check link error
    glLinkProgram( m_handle );
    GLint status = GL_FALSE, logLength = 0;
    glGetProgramiv( m_handle, GL_LINK_STATUS, &status );
    glGetProgramiv( m_handle, GL_INFO_LOG_LENGTH, &logLength );

    if ( logLength > 1 )
    {
        vector<char> buffer( logLength + 1 );
        glGetProgramInfoLog( m_handle, logLength, NULL, buffer.data() );
        std::cout << "[Log]: when linking program [" << file << "]\n";
        std::cout << buffer.data() << std::endl;
    }

    if ( status == GL_FALSE )
    {
        destroy();
        Com_PrintError( "[glsl] failed to create file" );
    }
}

void GlslProgram::createFromFiles( const char* vert, const char* frag, const char* geom )
{
    GLuint vertHandle = createShaderFromFile( vert, GL_VERTEX_SHADER );
    GLuint fragHandle = createShaderFromFile( frag, GL_FRAGMENT_SHADER );

    m_handle = glCreateProgram();

    glAttachShader( m_handle, vertHandle );
    glAttachShader( m_handle, fragHandle );

    GLuint geomHandle = GpuResource::NULL_HANDLE;
    if ( geom != nullptr )
    {
        geomHandle = createShaderFromFile( geom, GL_GEOMETRY_SHADER );
        glAttachShader( m_handle, geomHandle );
    }

    linkProgram( vert );

    glDeleteShader( vertHandle );
    glDeleteShader( fragHandle );
    if ( geomHandle != GpuResource::NULL_HANDLE )
        glDeleteShader( geomHandle );
}

void GlslProgram::createFromFile( const char* comp )
{
    GLuint compHandle = createShaderFromFile( comp, GL_COMPUTE_SHADER );

    m_handle = glCreateProgram();

    glAttachShader( m_handle, compHandle );

    linkProgram( comp );

    glDeleteShader( compHandle );
}

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
        std::cout << "[Warning] uniform [" << name << "] not found" << std::endl;
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

string processShader( const char* file )
{
    const string source = readAsciiFile( file );
    string out, line;
    stringstream ss( source );
    while ( std::getline( ss, line, '\n' ) )
    {
        const string pattern{ "/// #include " };
        if ( line.find( pattern ) == 0 )
        {
            Com_PrintWarning( "[glsl] fix it !" );
            string file = line.substr( pattern.length() );
            string path( DATA_DIR "shaders/" );
            path.append( file );
            out.append( readAsciiFile( path.c_str() ) );
        }
        else
            out.append( line );

        out.push_back( '\n' );
    }

    return out;
}

GLuint createShaderFromFile( const char* file, GLenum shaderType )
{
    GLuint handle = glCreateShader( shaderType );

    string source          = processShader( file );
    const char* sources[1] = { source.c_str() };
    glShaderSource( handle, 1, sources, NULL );
    glCompileShader( handle );

    GLint status = GL_FALSE, logLength = 0;
    glGetShaderiv( handle, GL_COMPILE_STATUS, &status );
    glGetShaderiv( handle, GL_INFO_LOG_LENGTH, &logLength );
    if ( logLength > 1 )
    {
        vector<char> buffer( logLength + 1 );
        glGetShaderInfoLog( handle, logLength, NULL, buffer.data() );
        std::cout << "[Log]: when compiling shader [" << file << "]\n";
        std::cout << buffer.data() << std::endl;
    }

    if ( status == GL_FALSE )
    {
        glDeleteShader( handle );
        core_assert( 0 );
    }

    return handle;
}

}  // namespace vct
