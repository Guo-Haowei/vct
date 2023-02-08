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
OpenGLMeshData g_quad;

// @TODO: refactor
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

    createGpuResources();

    InitializeGeometries( Com_GetScene() );

    auto createUBO = []( int slot ) {
        GLuint handle = 0;
        glGenBuffers( 1, &handle );
        glBindBufferBase( GL_UNIFORM_BUFFER, slot, handle );
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
        return handle;
    };
    m_uboDrawFrameConstant = createUBO( 0 );
    m_uboDrawBatchConstant = createUBO( 1 );

    return ( m_bInitialized = true );
}

void OpenGLGraphicsManager::Finalize()
{
    for ( auto &it : m_sceneGpuMeshes ) {
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
    m_sceneGpuMeshes.clear();

    destroyGpuResources();
    ImGui_ImplOpenGL3_Shutdown();
}

void OpenGLGraphicsManager::SetPipelineState( const std::shared_ptr<PipelineState> &pipeline_state )
{
    const OpenGLPipelineState *pPipelineState = dynamic_cast<const OpenGLPipelineState *>( pipeline_state.get() );

    // m_CurrentShader = pPipelineState->shaderProgram;

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

// @TODO: remove
#include "Graphics/r_cbuffers.h"

static void FillMaterialCB( const MaterialData *mat, MaterialCB &cb )
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

// HACK:
void OpenGLGraphicsManager::DrawBatch( const Frame & )
{
    // @TODO: culling
    const Frame &frame = m_frame;
    for ( auto &pDbc : frame.batchContexts ) {
        SetPerBatchConstants( *pDbc );

        const auto &dbc = dynamic_cast<const OpenGLDrawBatchContext &>( *pDbc );

        const MaterialData *matData = reinterpret_cast<MaterialData *>( pDbc->pEntity->m_material->gpuResource );

        FillMaterialCB( matData, g_materialCache.cache );
        g_materialCache.Update();

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

static std::shared_ptr<OpenGLMeshData> CreateMeshData( const MeshComponent& mesh )
{
    auto ret = std::make_shared<OpenGLMeshData>();

    const bool hasNormals = !mesh.normals.empty();
    const bool hasUVs = !mesh.uvs.empty();
    const bool hasTangent = !mesh.tangents.empty();
    const bool hasBitangent = !mesh.bitangents.empty();

    glGenVertexArrays( 1, &ret->vao );
    glGenBuffers( 2 + hasNormals + hasUVs + hasTangent + hasBitangent, &ret->ebo );
    glBindVertexArray( ret->vao );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ret->ebo );
    gl::BindToSlot( ret->vbos[0], 0, 3 );
    gl::NamedBufferStorage( ret->vbos[0], mesh.positions );
    if ( hasNormals ) {
        gl::BindToSlot( ret->vbos[1], 1, 3 );
        gl::NamedBufferStorage( ret->vbos[1], mesh.normals );
    }
    if ( hasUVs ) {
        gl::BindToSlot( ret->vbos[2], 2, 2 );
        gl::NamedBufferStorage( ret->vbos[2], mesh.uvs );
    }
    if ( hasTangent ) {
        gl::BindToSlot( ret->vbos[3], 3, 3 );
        gl::NamedBufferStorage( ret->vbos[3], mesh.tangents );
        gl::BindToSlot( ret->vbos[4], 4, 3 );
        gl::NamedBufferStorage( ret->vbos[4], mesh.bitangents );
    }

    gl::NamedBufferStorage( ret->ebo, mesh.indices );
    ret->count = static_cast<uint32_t>( mesh.indices.size() );

    glBindVertexArray( 0 );
    return ret;
}

void OpenGLGraphicsManager::InitializeGeometries( const Scene &scene )
{
    // create gpu meshes
    for ( const auto &mesh : scene.m_meshes ) {
        m_sceneGpuMeshes.emplace_back( CreateMeshData( *mesh.get() ) );
        mesh->gpuResource = m_sceneGpuMeshes.back().get();
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
