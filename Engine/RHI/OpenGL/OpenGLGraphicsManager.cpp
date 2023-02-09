#include "OpenGLGraphicsManager.hpp"

#include <set>

#include "OpenGLPipelineStateManager.hpp"
#include "imgui_impl_opengl3.h"

#include "Base/Asserts.h"
#include "Base/StringUtils.hpp"

#include "Core/com_dvars.h"
#include "Core/GlfwApplication.hpp"

#include "Graphics/gl_utils.h"
#include "GLFW/glfw3.h"

#include "Manager/BaseApplication.hpp"
#include "Manager/SceneManager.hpp"

#include "Graphics/MainRenderer.h"

// @TODO: refactor
OpenGLMeshData g_quad;
static void R_CreateQuad()
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
    // glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1);
}

static void APIENTRY debug_callback( GLenum, GLenum, unsigned int, GLenum, GLsizei, const char *, const void * );

bool OpenGLGraphicsManager::Initialize()
{
    if ( !GraphicsManager::Initialize() ) {
        return false;
    }

    if ( gladLoadGL() == 0 ) {
        LOG_FATAL( "[glad] failed to load gl functions" );
        return false;
    }

    LOG_DEBUG( "[opengl] renderer: %s", glGetString( GL_RENDERER ) );
    LOG_DEBUG( "[opengl] version: %s", glGetString( GL_VERSION ) );

    if ( Dvar_GetBool( r_debug ) ) {
        int flags;
        glGetIntegerv( GL_CONTEXT_FLAGS, &flags );
        if ( flags & GL_CONTEXT_FLAG_DEBUG_BIT ) {
            glEnable( GL_DEBUG_OUTPUT );
            glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
            glDebugMessageCallback( debug_callback, nullptr );
            glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
            LOG_DEBUG( "[opengl] debug callback enabled" );
        }
    }

    // @TODO: move to GraphicsManager
    auto pipelineStateManager = dynamic_cast<BaseApplication *>( m_pApp )->GetPipelineStateManager();

    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new ShadowMapPass( this, pipelineStateManager, &g_shadowRT, CLEAR_FLAG_DEPTH ) ) );
    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new VoxelizationPass( this, pipelineStateManager, nullptr, CLEAR_FLAG_NONE ) ) );
    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new ForwardPass( this, pipelineStateManager, nullptr, CLEAR_FLAG_COLOR_DPETH ) ) );
    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new OverlayPass( this, pipelineStateManager, nullptr, 0 ) ) );
    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new GuiPass( this, pipelineStateManager, nullptr, 0 ) ) );

    char version[256];
    Sprintf( version, "#version %d%d0 core", OPENGL_DEFAULT_VERSION_MAJOR, OPENGL_DEFAULT_VERSION_MINOR );
    if ( !ImGui_ImplOpenGL3_Init( version ) ) {
        LOG_FATAL( "OpenGLGraphicsManager::Initialize: ImGui_ImplOpenGL3_Init(\"%s\") failed!", version );
        return false;
    }

    if ( !ImGui_ImplOpenGL3_CreateDeviceObjects() ) {
        LOG_FATAL( "OpenGLGraphicsManager::Initialize: ImGui_ImplOpenGL3_CreateDeviceObjects() failed!" );
        return false;
    }

    // create render targets
    {
        int w = 0, h = 0;
        GetAppPointer()->GetFramebufferSize( w, h );

        const int res = Dvar_GetInt( r_shadowRes );
        ASSERT( is_power_of_two( res ) );

        // g_shadowRT.Create( NUM_CASCADES * res, res );
        g_shadowRT.Create( res, res );
    }

    R_CreateQuad();
    createGpuResources();

    auto createUBO = []( int slot ) {
        GLuint handle = 0;
        glGenBuffers( 1, &handle );
        glBindBufferBase( GL_UNIFORM_BUFFER, slot, handle );
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
        return handle;
    };

    m_uboDrawBatchConstant = createUBO( 0 );
    m_uboDrawFrameConstant = createUBO( 1 );

    // @TODO: refactor
    {
        GLuint cb = createUBO( 2 );
        PerSceneConstants cache;
        {
            constexpr float s = 0.14f;
            float h = 1.0f - s;
            int i = 0;
            cache.OverlayPositions[i++] = vec4( vec2( 1.0f - s, h ), vec2( s - 0.01f ) );
        }
        cache.ShadowMap = gl::MakeTextureResident( g_shadowRT.GetDepthTexture().GetHandle() );
        cache.VoxelAlbedoMap = gl::MakeTextureResident( m_albedoVoxel.GetHandle() );
        cache.VoxelNormalMap = gl::MakeTextureResident( m_normalVoxel.GetHandle() );

        glBindBuffer( GL_UNIFORM_BUFFER, cb );
        glBufferData( GL_UNIFORM_BUFFER, kSizePerSceneConstantBuffer, &cache, GL_DYNAMIC_DRAW );
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
    }

    return ( m_bInitialized = true );
}

void OpenGLGraphicsManager::Finalize()
{
    // @TODO: move to scene
    for ( auto &it : m_sceneMeshData ) {
        glDeleteVertexArrays( 1, &it->vao );
        for ( int i = 0; i < array_length( it->vbos ); ++i ) {
            if ( it->vbos[i] ) {
                glDeleteBuffers( 1, &it->vbos[i] );
            }
        }
        if ( it->ebo ) {
            glDeleteBuffers( 1, &it->ebo );
        }
    }
    m_sceneMeshData.clear();

    for ( auto &it : m_sceneTextures ) {
        GLuint textures[]{ (GLuint)it->albedoMap.handle, (GLuint)it->normalMap.handle, (GLuint)it->pbrMap.handle };
        glDeleteTextures( array_length( textures ), textures );
    }
    m_sceneTextures.clear();

    destroyGpuResources();
    ImGui_ImplOpenGL3_Shutdown();
}

void OpenGLGraphicsManager::SetPipelineState( const std::shared_ptr<PipelineState> &pipeline_state )
{
    const OpenGLPipelineState *pPipelineState = dynamic_cast<const OpenGLPipelineState *>( pipeline_state.get() );
    m_currentShader = pPipelineState->shaderProgram;

    // Set the color shader as the current shader program and set the matrices
    // that it will use for rendering.
    glUseProgram( pPipelineState->shaderProgram );

    switch ( pipeline_state->cullFaceMode ) {
        case CULL_FACE_MODE::NONE:
            glDisable( GL_CULL_FACE );
            break;
        case CULL_FACE_MODE::FRONT:
            glEnable( GL_CULL_FACE );
            glCullFace( GL_FRONT );
            break;
        case CULL_FACE_MODE::BACK:
            glEnable( GL_CULL_FACE );
            glCullFace( GL_BACK );
            break;
        default:
            UNREACHABLE();
            break;
    }

    switch ( pipeline_state->depthTestMode ) {
        case DEPTH_TEST_MODE::NONE:
            glDisable( GL_DEPTH_TEST );
            break;
        case DEPTH_TEST_MODE::LARGE:
            glEnable( GL_DEPTH_TEST );
            glDepthFunc( GL_GREATER );
            break;
        case DEPTH_TEST_MODE::LARGE_EQUAL:
            glEnable( GL_DEPTH_TEST );
            glDepthFunc( GL_GEQUAL );
            break;
        case DEPTH_TEST_MODE::LESS:
            glEnable( GL_DEPTH_TEST );
            glDepthFunc( GL_LESS );
            break;
        case DEPTH_TEST_MODE::LESS_EQUAL:
            glEnable( GL_DEPTH_TEST );
            glDepthFunc( GL_LEQUAL );
            break;
        case DEPTH_TEST_MODE::EQUAL:
            glEnable( GL_DEPTH_TEST );
            glDepthFunc( GL_EQUAL );
            break;
        case DEPTH_TEST_MODE::NOT_EQUAL:
            glEnable( GL_DEPTH_TEST );
            glDepthFunc( GL_NOTEQUAL );
            break;
        case DEPTH_TEST_MODE::NEVER:
            glEnable( GL_DEPTH_TEST );
            glDepthFunc( GL_NEVER );
            break;
        case DEPTH_TEST_MODE::ALWAYS:
            glEnable( GL_DEPTH_TEST );
            glDepthFunc( GL_ALWAYS );
            break;
        default:
            UNREACHABLE();
    }
}

void OpenGLGraphicsManager::DrawBatch( const Frame & )
{
    const Frame &frame = m_frame;
    for ( auto &it : frame.batchContexts ) {
        const auto &dbc = dynamic_cast<const OpenGLDrawBatchContext &>( *it );
        SetPerBatchConstants( dbc );

        const Entity &entity = *dbc.pEntity;
        const MaterialTextures &textures = *( entity.m_material->gpuResource );

        int textureOffset = 5;
        SetShaderParameter( "UniformAlbedoMap", textureOffset );
        glActiveTexture( GL_TEXTURE0 + textureOffset );
        glBindTexture( GL_TEXTURE_2D, (GLuint)textures.albedoMap.handle );

        ++textureOffset;
        SetShaderParameter( "UniformNormalMap", textureOffset );
        glActiveTexture( GL_TEXTURE0 + textureOffset );
        glBindTexture( GL_TEXTURE_2D, (GLuint)textures.normalMap.handle );

        ++textureOffset;
        SetShaderParameter( "UniformPBRMap", textureOffset );
        glActiveTexture( GL_TEXTURE0 + textureOffset );
        glBindTexture( GL_TEXTURE_2D, (GLuint)textures.pbrMap.handle );

        glBindVertexArray( dbc.vao );
        glDrawElements( dbc.mode, dbc.count, dbc.type, nullptr );
    }

    glBindVertexArray( 0 );
}

void OpenGLGraphicsManager::SetPerFrameConstants(
    const DrawFrameContext &context )
{
    glBindBuffer( GL_UNIFORM_BUFFER, m_uboDrawFrameConstant );

    const auto &constants = static_cast<const PerFrameConstants &>( context );

    glBufferData( GL_UNIFORM_BUFFER, kSizePerFrameConstantBuffer, &constants, GL_DYNAMIC_DRAW );

    glBindBuffer( GL_UNIFORM_BUFFER, 0 );
}

void OpenGLGraphicsManager::SetPerBatchConstants(
    const DrawBatchContext &context )
{
    glBindBuffer( GL_UNIFORM_BUFFER, m_uboDrawBatchConstant );

    const auto &constant = static_cast<const PerBatchConstants &>( context );

    glBufferData( GL_UNIFORM_BUFFER, kSizePerBatchConstantBuffer, &constant, GL_DYNAMIC_DRAW );

    glBindBuffer( GL_UNIFORM_BUFFER, 0 );
}

void OpenGLGraphicsManager::InitializeGeometries( const Scene &scene )
{
    auto createMeshData = []( const std::shared_ptr<MeshComponent> &mesh ) {
        auto ret = std::make_shared<OpenGLMeshData>();

        const bool hasNormals = !mesh->normals.empty();
        const bool hasUVs = !mesh->uvs.empty();
        const bool hasTangent = !mesh->tangents.empty();
        const bool hasBitangent = !mesh->bitangents.empty();

        glGenVertexArrays( 1, &ret->vao );
        glGenBuffers( 2 + hasNormals + hasUVs + hasTangent + hasBitangent, &ret->ebo );
        glBindVertexArray( ret->vao );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ret->ebo );
        gl::BindToSlot( ret->vbos[0], 0, 3 );
        gl::NamedBufferStorage( ret->vbos[0], mesh->positions );
        if ( hasNormals ) {
            gl::BindToSlot( ret->vbos[1], 1, 3 );
            gl::NamedBufferStorage( ret->vbos[1], mesh->normals );
        }
        if ( hasUVs ) {
            gl::BindToSlot( ret->vbos[2], 2, 2 );
            gl::NamedBufferStorage( ret->vbos[2], mesh->uvs );
        }
        if ( hasTangent ) {
            gl::BindToSlot( ret->vbos[3], 3, 3 );
            gl::NamedBufferStorage( ret->vbos[3], mesh->tangents );
            gl::BindToSlot( ret->vbos[4], 4, 3 );
            gl::NamedBufferStorage( ret->vbos[4], mesh->bitangents );
        }

        gl::NamedBufferStorage( ret->ebo, mesh->indices );
        ret->count = static_cast<uint32_t>( mesh->indices.size() );

        glBindVertexArray( 0 );
        return ret;
    };

    auto createMaterialTextures = [&scene]( const std::shared_ptr<MaterialComponent> &material ) {
        auto ret = std::make_shared<MaterialTextures>();

        auto uploadTexture = []( const std::shared_ptr<Image> &image ) {
            ASSERT( image->m_pPixels );

            GLenum format = GL_RGBA;
            switch ( image->m_pixelFormat ) {
                case PIXEL_FORMAT::RGBA8:
                    break;
                case PIXEL_FORMAT::RGB8:
                    format = GL_RGB;
                    break;
                default:
                    LOG_WARN( "Unsupported format %d", (int)image->m_pixelFormat );
                    break;
            }

            GLuint textureHandle = 0;
            glGenTextures( 1, &textureHandle );
            glBindTexture( GL_TEXTURE_2D, textureHandle );
            glTexImage2D( GL_TEXTURE_2D, 0, format, image->m_width, image->m_height, 0, format, GL_UNSIGNED_BYTE, image->m_pPixels );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
            glGenerateMipmap( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, 0 );

            Texture2D texture;
            texture.pixelFormat = image->m_pixelFormat;
            texture.format = format;
            texture.width = image->m_width;
            texture.height = image->m_height;
            texture.handle = textureHandle;
            return texture;
        };

        std::string &key = material->albedoTexture;
        if ( !key.empty() ) {
            ret->albedoMap = uploadTexture( scene.GetImage( key ) );
        }

        key = material->normalTexture;
        if ( !key.empty() ) {
            ret->normalMap = uploadTexture( scene.GetImage( key ) );
        }

        key = material->pbrTexture;
        if ( !key.empty() ) {
            ret->pbrMap = uploadTexture( scene.GetImage( key ) );
        }
        return ret;
    };

    // create meshes
    for ( const auto &mesh : scene.m_meshes ) {
        m_sceneMeshData.emplace_back( createMeshData( mesh ) );
        mesh->gpuResource = m_sceneMeshData.back().get();
    }

    // create materials
    for ( const auto &material : scene.m_materials ) {
        m_sceneTextures.emplace_back( createMaterialTextures( material ) );
        material->gpuResource = m_sceneTextures.back().get();
    }

    uint32_t batch_index = 0;
    for ( const auto &entity : scene.m_entities ) {
        if ( !( entity->m_flag & Entity::FLAG_GEOMETRY ) ) {
            continue;
        }

        const OpenGLMeshData *drawData = reinterpret_cast<OpenGLMeshData *>( entity->m_mesh->gpuResource );

        auto dbc = std::make_shared<OpenGLDrawBatchContext>();
        dbc->batchIndex = batch_index++;
        dbc->vao = drawData->vao;
        dbc->mode = GL_TRIANGLES;
        dbc->type = GL_UNSIGNED_INT;
        dbc->count = drawData->count;

        dbc->pEntity = entity.get();
        dbc->Model = mat4( 1 );

        m_frame.batchContexts.push_back( dbc );
    }
}

void OpenGLGraphicsManager::BeginFrame( Frame &frame )
{
    GraphicsManager::BeginFrame( frame );

    SetPerFrameConstants( frame.frameContexts );
}

void OpenGLGraphicsManager::EndFrame( Frame & )
{
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
}

void OpenGLGraphicsManager::Present()
{
    GLFWwindow *pGlfwWindow = reinterpret_cast<GLFWwindow *>( m_pApp->GetMainWindowHandler() );
    ASSERT( pGlfwWindow );
    glfwSwapBuffers( pGlfwWindow );
}

bool OpenGLGraphicsManager::SetShaderParameter( const char *param_name, const int32_t value )
{
    unsigned int location;

    location = glGetUniformLocation( m_currentShader, param_name );
    if ( location == -1 ) {
        return false;
    }
    glUniform1i( location, value );

    return true;
}

static void APIENTRY debug_callback(
    GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei,
    const char *message,
    const void * )
{
    using base::Level;

    const char *sourceStr = "other";
    const char *typeStr = "other";
    switch ( source ) {
        case GL_DEBUG_SOURCE_API: sourceStr = "api"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceStr = "window system"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "shader compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: sourceStr = "third party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: sourceStr = "application"; break;
        default: break;
    }

    switch ( type ) {
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

    Level level = Level::Warn;
    const char *severityStr = "low";
    switch ( severity ) {
        case GL_DEBUG_SEVERITY_HIGH:
            severityStr = "high";
            level = Level::Error;
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            severityStr = "medium";
            level = Level::Warn;
            break;
        default:
            break;
    }

    // TODO: properly disable repeated warnings
    static std::set<int> sSet{};

    if ( sSet.find( id ) == sSet.end() ) {
        base::Log( level, "[opengl] %s\n\t| id: %d | source: %s | type: %s | severity: %s |", message, id, sourceStr, typeStr, severityStr );
        sSet.insert( id );
    }
}