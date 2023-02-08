#include "gl_utils.h"

#include <glad/glad.h>

#include <filesystem>
#include <set>
#include <sstream>

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "cbuffer.glsl"

namespace gl {

//------------------------------------------------------------------------------
// Constant Buffer
//------------------------------------------------------------------------------
GLuint CreateAndBindConstantBuffer( int slot, size_t sizeInByte )
{
    GLuint handle = 0;
    glGenBuffers( 1, &handle );
    glBindBuffer( GL_UNIFORM_BUFFER, handle );
    glBufferData( GL_UNIFORM_BUFFER, sizeInByte, nullptr, GL_DYNAMIC_DRAW );
    glBindBuffer( GL_UNIFORM_BUFFER, 0 );
    glBindBufferBase( GL_UNIFORM_BUFFER, slot, handle );
    LOG_DEBUG( "[opengl] created buffer of size %zu (slot %d)", sizeInByte, slot );
    glBindBuffer( GL_UNIFORM_BUFFER, 0 );
    return handle;
}

void UpdateConstantBuffer( GLuint handle, const void *ptr, size_t sizeInByte )
{
    // glMapBuffer( mHandle, 0 );
    glBindBuffer( GL_UNIFORM_BUFFER, handle );
    glBufferData( GL_UNIFORM_BUFFER, sizeInByte, ptr, GL_DYNAMIC_DRAW );
    glBindBuffer( GL_UNIFORM_BUFFER, 0 );
}

//------------------------------------------------------------------------------
// DebugCallback
//------------------------------------------------------------------------------

}  // namespace gl