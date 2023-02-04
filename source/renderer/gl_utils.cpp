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

void FillMaterialCB( const MaterialData *mat, MaterialCB &cb )
{
    cb.AlbedoColor = mat->albedoColor;
    cb.Metallic = mat->metallic;
    cb.Roughness = mat->roughness;
    cb.HasAlbedoMap = mat->albedoMap.GetHandle() != 0;
    cb.HasNormalMap = mat->materialMap.GetHandle() != 0;
    cb.HasPbrMap = mat->materialMap.GetHandle() != 0;
    cb.TextureMapIdx = mat->textureMapIdx;
    cb.ReflectPower = mat->reflectPower;
}

namespace gl {

//------------------------------------------------------------------------------
// Shader
//------------------------------------------------------------------------------

static std::string ProcessShader( const std::string &source )
{
    std::string result;
    std::stringstream ss( source );
    for ( std::string line; std::getline( ss, line ); ) {
        constexpr const char pattern[] = "#include";
        if ( line.find( pattern ) == 0 ) {
            std::string include = line.substr( line.find( '"' ) );
            ASSERT( include.front() == '"' && include.back() == '"' );
            include.pop_back();
            SystemFileWrapper fhandle( g_fileMgr->OpenRead( include.c_str() + 1, "source/shaders" ) );
            std::vector<char> extra;
            if ( fhandle.Read( extra ) != SystemFile::Result::Ok ) {
                LOG_ERROR( "[filesystem] failed to read shader '%s'", include.c_str() );
            }
            result.append( extra.data() );
        }
        else {
            result.append( line );
        }

        result.push_back( '\n' );
    }

    return result;
}

class ShaderHandleWrapper {
    GLuint handle_ = 0;

public:
    ShaderHandleWrapper( GLuint handle )
        : handle_( handle ) {}

    ~ShaderHandleWrapper()
    {
        if ( handle_ ) {
            glDeleteShader( handle_ );
        }
    }

    operator GLuint()
    {
        ASSERT( handle_ );
        return handle_;
    }
};

static GLuint CreateShader( const char *file, GLenum type )
{
    // @TODO: resource management
    SystemFileWrapper fhandle( g_fileMgr->OpenRead( file, "source/shaders" ) );
    std::string source;
    const SystemFile::Result result = fhandle.Read( source );
    if ( result != SystemFile::Result::Ok ) {
        LOG_ERROR( "[filesystem] failed to read shader '%s'", file );
        return 0;
    }

    std::string fullsource = ProcessShader( source );
    constexpr const char extras[] =
        "#version 460 core\n"
        "#extension GL_NV_gpu_shader5 : require\n"
        "#extension GL_NV_shader_atomic_float : enable\n"
        "#extension GL_NV_shader_atomic_fp16_vector : enable\n"
        "#extension GL_ARB_bindless_texture : require\n"
        "";
    const char *sources[] = { extras, fullsource.c_str() };

    GLuint shader = glCreateShader( type );
    glShaderSource( shader, array_length( sources ), sources, nullptr );
    glCompileShader( shader );

    GLint status = GL_FALSE, length = 0;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );
    if ( length > 0 ) {
        std::vector<char> buffer( length + 1 );
        glGetShaderInfoLog( shader, length, nullptr, buffer.data() );
        LOG_ERROR( "[glsl] failed to compile shader '%s'\ndetails:\n%s", file, buffer.data() );
    }

    if ( status == GL_FALSE ) {
        glDeleteShader( shader );
        return 0;
    }

    return shader;
}

GLuint CreateProgram( const ProgramCreateInfo &info )
{
    GLuint program = 0;
    const char *name = "<unknown>";

    GLuint vs, ps, gs, cs;

    if ( !info.cs.empty() ) {
        ASSERT( info.vs.empty() );
        ASSERT( info.ps.empty() );
        ASSERT( info.gs.empty() );

        LOG_INFO( "compiling compute (%s) pipeline", info.cs.c_str() );
        program = glCreateProgram();
        name = info.cs.c_str();
        cs = CreateShader( info.cs.c_str(), GL_COMPUTE_SHADER );
        glAttachShader( program, cs );
    }
    else if ( info.vs[0] && info.ps[0] ) {
        LOG_INFO( "compiling vertex (%s), geometry(%s), pixel(%s)", info.vs.c_str(), info.gs.c_str(), info.ps.c_str() );

        program = glCreateProgram();
        name = info.vs.c_str();

        vs = CreateShader( info.vs.c_str(), GL_VERTEX_SHADER );
        glAttachShader( program, vs );
        ps = CreateShader( info.ps.c_str(), GL_FRAGMENT_SHADER );
        glAttachShader( program, ps );
        if ( !info.gs.empty() ) {
            gs = CreateShader( info.gs.c_str(), GL_GEOMETRY_SHADER );
            glAttachShader( program, gs );
        }
    }

    ASSERT( program );

    glLinkProgram( program );
    GLint status = GL_FALSE, length = 0;
    glGetProgramiv( program, GL_LINK_STATUS, &status );
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &length );
    if ( length > 0 ) {
        std::vector<char> buffer( length + 1 );
        glGetProgramInfoLog( program, length, nullptr, buffer.data() );
        LOG_ERROR( "[glsl] failed to link program '%s'\ndetails:\n%s", name, buffer.data() );
    }

    if ( status == GL_FALSE ) {
        glDeleteProgram( program );
        return 0;
    }

    return program;
}

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