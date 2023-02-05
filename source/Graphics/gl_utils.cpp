#include "gl_utils.h"

#include <glad/glad.h>

#include <filesystem>
#include <set>
#include <sstream>

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Core/FileManager.h"

#include "Core/com_dvars.h"
#include "shaders/cbuffer.glsl"
#include "universal/dvar_api.h"

static MeshData g_quad;

void R_CreateQuad()
{
    // clang-format off
    float points[] = { -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, };
    // clang-format on
    glGenVertexArrays( 1, &g_quad.vao );
    glGenBuffers( 1, g_quad.vbos );
    glBindVertexArray( g_quad.vao );

    glBindBuffer( GL_ARRAY_BUFFER, g_quad.vbos[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( points ), points, GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof( float ), 0 );
    glEnableVertexAttribArray( 0 );
}

void R_DrawQuad()
{
    ASSERT( g_quad.vao );
    glBindVertexArray( g_quad.vao );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
}

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