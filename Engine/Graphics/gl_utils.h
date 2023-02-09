#pragma once
#include <glad/glad.h>

#include <string>
#include <vector>

#include "GpuTexture.h"

#include "Base/Logger.h"
#include "Core/GeomMath.hpp"

#include "cbuffer.shader.hpp"

namespace gl {

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

//------------------------------------------------------------------------------
// Constant Buffer
//------------------------------------------------------------------------------

GLuint CreateAndBindConstantBuffer( int slot, size_t sizeInByte );
void UpdateConstantBuffer( GLuint handle, const void* ptr, size_t sizeInByte );

static inline GLuint64 MakeTextureResident( GLuint texture )
{
    GLuint64 ret = glGetTextureHandleARB( texture );
    glMakeTextureHandleResidentARB( ret );
    return ret;
}

}  // namespace gl
