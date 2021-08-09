#pragma once
#include <glad/glad.h>

#include <string>
#include <vector>

#include "GpuTexture.h"
#include "universal/core_math.h"
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

struct MeshData {
    GLuint vao     = 0;
    GLuint ebo     = 0;
    GLuint vbos[5] = { 0, 0, 0, 0, 0 };
    uint32_t count = 0;
};

struct MaterialData {
    GpuTexture albedoMap;
    GpuTexture materialMap;
    GpuTexture normalMap;
    vec4 albedoColor;
    float metallic;
    float roughness;
    float reflectPower;
    int textureMapIdx;
};

void R_CreateQuad();
void R_DrawQuad();

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

GLuint CreateProgram( const ProgramCreateInfo& info );

class Program {
   public:
    void Destroy()
    {
        if ( mHandle )
        {
            glDeleteProgram( mHandle );
        }

        mHandle = 0;
    }

    void Use() const
    {
        glUseProgram( mHandle );
    }
    void Stop() const
    {
        glUseProgram( 0 );
    }

    Program& operator=( GLuint program )
    {
        mHandle = program;
        return *this;
    }

   private:
    GLuint mHandle = 0;
};

//------------------------------------------------------------------------------
// Constant Buffer
//------------------------------------------------------------------------------

GLuint CreateAndBindConstantBuffer( int slot, size_t sizeInByte );
void UpdateConstantBuffer( GLuint handle, const void* ptr, size_t sizeInByte );

template<typename T>
struct ConstantBuffer {
    void Destroy()
    {
        if ( mHandle != 0 )
        {
            Com_Printf( "[opengl] destroy cbuffer %u", mHandle );
            glDeleteBuffers( 1, &mHandle );
        }
        mHandle = 0;
    }

    void CreateAndBind( int slot )
    {
        mHandle = CreateAndBindConstantBuffer( slot, sizeof( T ) );
    }

    void Update()
    {
        UpdateConstantBuffer( mHandle, &cache, sizeof( T ) );
    }

    T cache;
    GLuint mHandle = 0;
};

static inline GLuint64 MakeTextureResident( GLuint texture )
{
    GLuint64 ret = glGetTextureHandleARB( texture );
    glMakeTextureHandleResidentARB( ret );
    return ret;
}

}  // namespace gl
