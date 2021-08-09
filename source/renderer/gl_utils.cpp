#include "gl_utils.h"

#include <glad/glad.h>

#include <filesystem>
#include <set>
#include <sstream>

#include "common/com_dvars.h"
#include "common/com_filesystem.h"
#include "shaders/cbuffer.glsl"
#include "universal/core_assert.h"
#include "universal/dvar_api.h"
#include "universal/print.h"
#include "universal/universal.h"

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
    core_assert( g_quad.vao );
    glBindVertexArray( g_quad.vao );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
}

void FillMaterialCB( const MaterialData *mat, MaterialCB &cb )
{
    cb.AlbedoColor   = mat->albedoColor;
    cb.Metallic      = mat->metallic;
    cb.Roughness     = mat->roughness;
    cb.HasAlbedoMap  = mat->albedoMap.GetHandle() != 0;
    cb.HasNormalMap  = mat->materialMap.GetHandle() != 0;
    cb.HasPbrMap     = mat->materialMap.GetHandle() != 0;
    cb.TextureMapIdx = mat->textureMapIdx;
    cb.ReflectPower  = mat->reflectPower;
}

namespace gl {

static void APIENTRY DebugCallback( GLenum, GLenum, unsigned int, GLenum, GLsizei, const char *, const void * );

bool Init()
{
    if ( gladLoadGL() == 0 )
    {
        Com_PrintFatal( "[glad] failed to load gl functions" );
        return false;
    }

    Com_Printf( "[opengl] renderer: %s", glGetString( GL_RENDERER ) );
    Com_Printf( "[opengl] version: %s", glGetString( GL_VERSION ) );

    if ( Dvar_GetBool( r_debug ) )
    {
        int flags;
        glGetIntegerv( GL_CONTEXT_FLAGS, &flags );
        if ( flags & GL_CONTEXT_FLAG_DEBUG_BIT )
        {
            glEnable( GL_DEBUG_OUTPUT );
            glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
            glDebugMessageCallback( gl::DebugCallback, nullptr );
            glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
            Com_Printf( "[opengl] debug callback enabled" );
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Shader
//------------------------------------------------------------------------------

static std::string ProcessShader( const std::string &source )
{
    std::string result;
    std::stringstream ss( source );
    for ( std::string line; std::getline( ss, line ); )
    {
        constexpr const char pattern[] = "#include";
        if ( line.find( pattern ) == 0 )
        {
            std::string include = line.substr( line.find( '"' ) );
            core_assert( include.front() == '"' && include.back() == '"' );
            include.pop_back();
            ComFileWrapper fhandle( Com_FsOpenRead( include.c_str() + 1, "source/shaders" ) );
            std::vector<char> extra;
            if ( fhandle.Read( extra ) != ComFile::Result::Ok )
            {
                Com_PrintError( "[filesystem] failed to read shader '%s'", include );
            }
            result.append( extra.data() );
        }
        else
        {
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
        if ( handle_ )
        {
            glDeleteShader( handle_ );
        }
    }

    operator GLuint()
    {
        core_assert( handle_ );
        return handle_;
    }
};

static GLuint CreateShader( const char *file, GLenum type )
{
    ComFileWrapper fhandle( Com_FsOpenRead( file, "source/shaders" ) );
    std::string source;
    const ComFile::Result result = fhandle.Read( source );
    if ( result != ComFile::Result::Ok )
    {
        Com_PrintError( "[filesystem] failed to read shader '%s'", file );
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
    if ( length > 0 )
    {
        std::vector<char> buffer( length + 1 );
        glGetShaderInfoLog( shader, length, nullptr, buffer.data() );
        Com_PrintError( "[glsl] failed to compile shader '%s'\ndetails:\n%s", file, buffer.data() );
    }

    if ( status == GL_FALSE )
    {
        glDeleteShader( shader );
        return 0;
    }

    return shader;
}

GLuint CreateProgram( const ProgramCreateInfo &info )
{
    GLuint program   = 0;
    const char *name = "<unknown>";

    GLuint vs, ps, gs, cs;

    if ( !info.cs.empty() )
    {
        core_assert( info.vs.empty() );
        core_assert( info.ps.empty() );
        core_assert( info.gs.empty() );

        Com_PrintInfo( "compiling compute (%s) pipeline", info.cs.c_str() );
        program = glCreateProgram();
        name    = info.cs.c_str();
        cs      = CreateShader( info.cs.c_str(), GL_COMPUTE_SHADER );
        glAttachShader( program, cs );
    }
    else if ( info.vs[0] && info.ps[0] )
    {
        Com_PrintInfo( "compiling vertex (%s), geometry(%s), pixel(%s)", info.vs.c_str(), info.gs.c_str(), info.ps.c_str() );

        program = glCreateProgram();
        name    = info.vs.c_str();

        vs = CreateShader( info.vs.c_str(), GL_VERTEX_SHADER );
        glAttachShader( program, vs );
        ps = CreateShader( info.ps.c_str(), GL_FRAGMENT_SHADER );
        glAttachShader( program, ps );
        if ( !info.gs.empty() )
        {
            gs = CreateShader( info.gs.c_str(), GL_GEOMETRY_SHADER );
            glAttachShader( program, gs );
        }
    }

    core_assert( program );

    glLinkProgram( program );
    GLint status = GL_FALSE, length = 0;
    glGetProgramiv( program, GL_LINK_STATUS, &status );
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &length );
    if ( length > 0 )
    {
        std::vector<char> buffer( length + 1 );
        glGetProgramInfoLog( program, length, nullptr, buffer.data() );
        Com_PrintError( "[glsl] failed to link program '%s'\ndetails:\n%s", name, buffer.data() );
    }

    if ( status == GL_FALSE )
    {
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
    Com_Printf( "[opengl] created buffer of size %zu (slot %d)", sizeInByte, slot );
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

static void APIENTRY DebugCallback(
    GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char *message,
    const void *userParam )
{
    using detail::Level;

    const char *sourceStr = "other";
    const char *typeStr   = "other";
    switch ( source )
    {
        case GL_DEBUG_SOURCE_API: sourceStr = "api"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceStr = "window system"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "shader compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: sourceStr = "third party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: sourceStr = "application"; break;
        default: break;
    }

    switch ( type )
    {
        case GL_DEBUG_TYPE_ERROR: typeStr = "error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "depracated behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeStr = "undefined behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY: typeStr = "portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: typeStr = "performance"; break;
        case GL_DEBUG_TYPE_MARKER: typeStr = "marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: typeStr = "push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP: typeStr = "pop Group"; break;
        case GL_DEBUG_TYPE_OTHER: typeStr = "other"; break;
        default: break;
    }

    Level level             = Level::Warning;
    const char *severityStr = "low";
    switch ( severity )
    {
        case GL_DEBUG_SEVERITY_HIGH:
            severityStr = "high";
            level       = Level::Error;
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            severityStr = "medium";
            level       = Level::Warning;
            break;
        default:
            break;
    }

    // TODO: properly disable repeated warnings
    static std::set<int> sSet{ 131185 };

    if ( sSet.find( id ) == sSet.end() )
    {
        detail::Print( level, "[opengl] %s\n\t| id: %d | source: %s | type: %s | severity: %s |", message, id, sourceStr, typeStr, severityStr );
        sSet.insert( id );
    }

    if ( static_cast<int>( level ) >= static_cast<int>( Level::Error ) )
    {
        __debugbreak();
    }
}

}  // namespace gl