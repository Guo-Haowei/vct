#include "OpenGLPipelineStateManager.hpp"

#include <sstream>

#include "glad/glad.h"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Manager/AssetLoader.hpp"
#include "Manager/BaseApplication.hpp"

using std::pair;
using std::string;
using std::vector;

using ShaderSourceList = std::vector<std::pair<uint32_t, std::string>>;

static std::string ProcessShader( const std::string &source )
{
    AssetLoader assetLoader;
    assetLoader.AddSearchPath( "Engine/Shader/GLSL" );
    assetLoader.AddSearchPath( "Engine/Graphics/" );

    string result;
    std::stringstream ss( source );
    for ( std::string line; std::getline( ss, line ); ) {
        constexpr const char pattern[] = "#include";
        if ( line.find( pattern ) == 0 ) {
            string include = line.substr( line.find( '"' ) );
            ASSERT( include.front() == '"' && include.back() == '"' );
            include.pop_back();
            constexpr char cbufferFile[] = "cbuffer.glsl";

            vector<char> includeSource = assetLoader.SyncOpenAndReadText( include.c_str() + 1 );
            result.append( includeSource.data() );
        }
        else {
            result.append( line );
        }

        result.push_back( '\n' );
    }

    return result;
}

static bool LoadShaderFromFile( const char *shader_name, const uint32_t shader_type, uint32_t &shader_handle )
{
    AssetLoader assetLoader;
    assetLoader.AddSearchPath( "Engine/Shader/GLSL" );

    string fileName( shader_name );
    fileName.append( ".glsl" );
    vector<char> sourceBuffer = assetLoader.SyncOpenAndReadText( fileName.c_str() );
    ASSERT( sourceBuffer.data() );

    constexpr const char extras[] =
        "#version 460 core\n"
        "#extension GL_NV_gpu_shader5 : require\n"
        "#extension GL_NV_shader_atomic_float : enable\n"
        "#extension GL_NV_shader_atomic_fp16_vector : enable\n"
        "#extension GL_ARB_bindless_texture : require\n"
        "";
    string preprocessedSource = ProcessShader( sourceBuffer.data() );

    const char *sources[] = { extras, preprocessedSource.c_str() };
    shader_handle = glCreateShader( shader_type );
    glShaderSource( shader_handle, array_length( sources ), sources, nullptr );
    glCompileShader( shader_handle );

    GLint status = GL_FALSE;
    GLint length = 0;
    glGetShaderiv( shader_handle, GL_COMPILE_STATUS, &status );
    glGetShaderiv( shader_handle, GL_INFO_LOG_LENGTH, &length );
    if ( length > 0 ) {
        std::vector<char> buffer( length + 1 );
        glGetShaderInfoLog( shader_handle, length, nullptr, buffer.data() );
        LOG_FATAL( "[glsl] failed to compile shader '%s'\ndetails:\n%s", fileName.c_str(), buffer.data() );
    }

    if ( status == GL_FALSE ) {
        glDeleteShader( shader_handle );
        return false;
    }

    return true;
}

static bool LoadShaderProgram( const ShaderSourceList &source, uint32_t &shaderProgram )
{
    // Create a shader program object.
    shaderProgram = glCreateProgram();

    for ( auto it = source.cbegin(); it != source.cend(); it++ ) {
        if ( !it->second.empty() ) {
            GLuint shader;
            if ( !LoadShaderFromFile( ( it->second ).c_str(), it->first, shader ) ) {
                return false;
            }

            // Attach the shader to the program object.
            glAttachShader( shaderProgram, shader );
            glDeleteShader( shader );
        }
    }

    // Link the shader program.
    glLinkProgram( shaderProgram );

    // Check the status of the link.
    GLint status = GL_FALSE;
    GLint length = 0;
    glGetProgramiv( shaderProgram, GL_LINK_STATUS, &status );
    glGetProgramiv( shaderProgram, GL_INFO_LOG_LENGTH, &length );
    if ( length > 0 ) {
        std::vector<char> buffer( length + 1 );
        glGetProgramInfoLog( shaderProgram, length, nullptr, buffer.data() );
        LOG_FATAL( "[glsl] failed to link program\ndetails:\n%s", buffer.data() );
        return false;
    }

    return true;
}

bool OpenGLPipelineStateManager::InitializePipelineState( PipelineState **ppPipelineState )
{
    OpenGLPipelineState *pnew_state = new OpenGLPipelineState( **ppPipelineState );
    ShaderSourceList list;

    if ( !( *ppPipelineState )->vertexShaderName.empty() ) {
        list.emplace_back( GL_VERTEX_SHADER, ( *ppPipelineState )->vertexShaderName );
    }

    if ( !( *ppPipelineState )->pixelShaderName.empty() ) {
        list.emplace_back( GL_FRAGMENT_SHADER, ( *ppPipelineState )->pixelShaderName );
    }

    if ( !( *ppPipelineState )->geometryShaderName.empty() ) {
        list.emplace_back( GL_GEOMETRY_SHADER, ( *ppPipelineState )->geometryShaderName );
    }

    if ( !( *ppPipelineState )->computeShaderName.empty() ) {
        list.emplace_back( GL_COMPUTE_SHADER, ( *ppPipelineState )->computeShaderName );
    }

    bool result = LoadShaderProgram( list, pnew_state->shaderProgram );

    *ppPipelineState = pnew_state;
    return result;
}

void OpenGLPipelineStateManager::DestroyPipelineState( PipelineState &pipelineState )
{
    OpenGLPipelineState *pPipelineState = dynamic_cast<OpenGLPipelineState *>( &pipelineState );
    glDeleteProgram( pPipelineState->shaderProgram );
}
