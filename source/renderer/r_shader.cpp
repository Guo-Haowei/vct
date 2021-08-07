#include "r_shader.h"

#include <vector>

#include "gl_utils.h"
#include "universal/core_assert.h"

static std::vector<GlslProgram> g_shaderCache;

void R_CreateShaderPrograms()
{
    g_shaderCache.resize( static_cast<int>( ProgramType::COUNT ) );
    g_shaderCache[static_cast<int>( ProgramType::LINE3D )].Create(
        gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "editor/line3d" ) ) );
    g_shaderCache[static_cast<int>( ProgramType::IMAGE2D )].Create(
        gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "editor/image" ) ) );
    g_shaderCache[static_cast<int>( ProgramType::SHADOW )].Create(
        gl::CreateShaderProgram( ProgramCreateInfo::VSPS( "depth" ) ) );
}

void R_DestroyShaderPrograms()
{
    for ( auto& program : g_shaderCache )
    {
        program.Destroy();
    }
}

const GlslProgram& R_GetShaderProgram( ProgramType type )
{
    core_assert( static_cast<int>( type ) < g_shaderCache.size() );
    return g_shaderCache[static_cast<int>( type )];
}