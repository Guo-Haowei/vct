#pragma once
#include <glad/glad.h>

#include <string>
#include <vector>

#include "universal/print.h"

static constexpr int kMaxShaderName = 128;

struct ProgramCreateInfo {
    std::string vs;
    std::string ps;
    std::string gs;
    std::string cs;

    static ProgramCreateInfo VSPS( const std::string& name )
    {
        ProgramCreateInfo info;
        info.vs = name + ".vert";
        info.ps = name + ".frag";
        return info;
    }

    static ProgramCreateInfo VSGSPS( const std::string& name )
    {
        ProgramCreateInfo info;
        info.vs = name + ".vert";
        info.gs = name + ".geom";
        info.ps = name + ".frag";
        return info;
    }

    static ProgramCreateInfo VSPS( const std::string& vs, const std::string& ps )
    {
        ProgramCreateInfo info;
        info.vs = vs + ".vert";
        info.ps = ps + ".frag";
        return info;
    }

    static ProgramCreateInfo CS( const std::string& cs )
    {
        ProgramCreateInfo info;
        info.cs = cs + ".comp";
        return info;
    }
};

namespace gl {

[[nodiscard]] bool Init();

template<typename T>
void NamedBufferStorage( GLuint buffer, const std::vector<T>& data )
{
    glNamedBufferStorage( buffer, sizeof( T ) * data.size(), data.data(), 0 );
}

static inline void BindToSlot( GLuint buffer, int slot, int size )
{
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glVertexAttribPointer( slot, size, GL_FLOAT, GL_FALSE, size * sizeof( float ), 0 );
    glEnableVertexAttribArray( slot );
}

GLuint CreateShaderProgram( const ProgramCreateInfo& info );

//------------------------------------------------------------------------------
// Constant Buffer
//------------------------------------------------------------------------------

template<typename T>
struct ConstantBuffer {
    void Destroy()
    {
        if ( handle_ != 0 )
        {
            Com_Printf( "[opengl] destroy cbuffer %u", handle_ );
            glDeleteBuffers( 1, &handle_ );
        }
        handle_ = 0;
    }

    void CreateAndBind( int slot )
    {
        glGenBuffers( 1, &handle_ );
        glBindBuffer( GL_UNIFORM_BUFFER, handle_ );
        glBufferData( GL_UNIFORM_BUFFER, sizeof( T ), 0, GL_DYNAMIC_DRAW );
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
        glBindBufferBase( GL_UNIFORM_BUFFER, slot, handle_ );
        Com_Printf( "[opengl] created buffer of size %zu (slot %d)", sizeof( T ), slot );
    }

    void Update()
    {
        glBindBuffer( GL_UNIFORM_BUFFER, handle_ );
        glBufferData( GL_UNIFORM_BUFFER, sizeof( T ), &cache, GL_DYNAMIC_DRAW );
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
    }

    T cache;
    GLuint handle_;
};

}  // namespace gl
