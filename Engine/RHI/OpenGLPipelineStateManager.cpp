#include "OpenGLPipelineStateManager.hpp"

#include <sstream>
#include <vector>

#include "glad/glad.h"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Interface/IAssetLoader.hpp"
#include "Manager/BaseApplication.hpp"

using std::pair;
using std::string;
using std::vector;

static std::string ProcessShader( const std::string &source, IAssetLoader *assetLoader )
{
    string result;
    std::stringstream ss( source );
    for ( std::string line; std::getline( ss, line ); ) {
        constexpr const char pattern[] = "#include";
        if ( line.find( pattern ) == 0 ) {
            string include = line.substr( line.find( '"' ) );
            ASSERT( include.front() == '"' && include.back() == '"' );
            include.pop_back();
            constexpr char cbufferFile[] = "cbuffer.glsl";

            vector<char> includeSource = assetLoader->SyncOpenAndReadText( include.c_str() + 1 );
            result.append( includeSource.data() );
        }
        else {
            result.append( line );
        }

        result.push_back( '\n' );
    }

    return result;
}

static bool LoadShaderFromFile( const char *file, const GLenum shaderType, GLuint &shader, IAssetLoader *assetLoader )
{
    string filename( file );
    filename.append( ".glsl" );
    vector<char> source = assetLoader->SyncOpenAndReadText( filename.c_str() );
    ASSERT( source.data() );

    constexpr const char extras[] =
        "#version 460 core\n"
        "#extension GL_NV_gpu_shader5 : require\n"
        "#extension GL_NV_shader_atomic_float : enable\n"
        "#extension GL_NV_shader_atomic_fp16_vector : enable\n"
        "#extension GL_ARB_bindless_texture : require\n"
        "";
    string fullsource = ProcessShader( source.data(), assetLoader );

    const char *sources[] = { extras, fullsource.c_str() };
    shader = glCreateShader( shaderType );
    glShaderSource( shader, array_length( sources ), sources, nullptr );
    glCompileShader( shader );

    GLint status = GL_FALSE;
    GLint length = 0;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );
    if ( length > 0 ) {
        std::vector<char> buffer( length + 1 );
        glGetShaderInfoLog( shader, length, nullptr, buffer.data() );
        LOG_FATAL( "[glsl] failed to compile shader '%s'\ndetails:\n%s", filename.c_str(), buffer.data() );
    }

    if ( status == GL_FALSE ) {
        glDeleteShader( shader );
        return false;
    }

    return true;
}

typedef vector<pair<GLenum, string>> ShaderSourceList;
static bool LoadShaderProgram( const ShaderSourceList &source, GLuint &shaderProgram, IAssetLoader *assetLoader )
{
    // Create a shader program object.
    shaderProgram = glCreateProgram();

    for ( auto it = source.cbegin(); it != source.cend(); it++ ) {
        if ( !it->second.empty() ) {
            GLuint shader;
            if ( !LoadShaderFromFile( ( it->second ).c_str(), it->first, shader, assetLoader ) ) {
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

bool OpenGLPipelineStateManager::InitializePipelineState(
    PipelineState **ppPipelineState )
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

    auto assetLoader = dynamic_cast<BaseApplication *>( m_pApp )->GetAssetLoader();
    bool result = LoadShaderProgram( list, pnew_state->shaderProgram, assetLoader );

    *ppPipelineState = pnew_state;
    return result;
}

void OpenGLPipelineStateManager::DestroyPipelineState(
    PipelineState &pipelineState )
{
    OpenGLPipelineState *pPipelineState = dynamic_cast<OpenGLPipelineState *>( &pipelineState );
    glDeleteProgram( pPipelineState->shaderProgram );
}
