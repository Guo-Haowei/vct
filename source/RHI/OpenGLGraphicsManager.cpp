#include "OpenGLGraphicsManager.hpp"

#include <set>

#include "Base/Asserts.h"

#include "Core/com_dvars.h"

#include "OpenGLPipelineStateManager.hpp"

#include "Graphics/gl_utils.h"

#include "Graphics/imgui_impl_opengl3.h"
#include "Core/WindowManager.h"

// @TODO: remove
#include "DrawPass/ShadowMapPass.hpp"
#include "DrawPass/SSAOPass.hpp"
#include "DrawPass/GBufferPass.hpp"
#include "DrawPass/DeferredPass.hpp"
#include "DrawPass/OverlayPass.hpp"
#include "DrawPass/VoxelizationPass.hpp"
#include "DrawPass/GuiPass.hpp"

GraphicsManager *g_gfxMgr = new OpenGLGraphicsManager();

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
    ASSERT( g_pPipelineStateManager );
    IPipelineStateManager *psm = g_pPipelineStateManager;

    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new ShadowMapPass( this, psm, &g_shadowRT, CLEAR_FLAG_DEPTH ) ) );
    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new VoxelizationPass( this, psm, nullptr, CLEAR_FLAG_NONE ) ) );
    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new GBufferPass( this, psm, &g_gbufferRT, CLEAR_FLAG_COLOR_DPETH ) ) );
    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new SSAOPass( this, psm, &g_ssaoRT, CLEAR_FLAG_COLOR ) ) );
    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new DeferredPass( this, psm, &g_finalImageRT, CLEAR_FLAG_COLOR ) ) );
    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new OverlayPass( this, psm, nullptr, CLEAR_FLAG_COLOR_DPETH ) ) );
    m_drawPasses.emplace_back( std::shared_ptr<BaseDrawPass>( new GuiPass( this, psm, nullptr, 0 ) ) );

    return ( m_bInitialized = true );
}

void OpenGLGraphicsManager::Finalize()
{
    m_bInitialized = false;
}

void OpenGLGraphicsManager::SetPipelineState( const std::shared_ptr<PipelineState> &pipelineState )
{
    const OpenGLPipelineState *pPipelineState = dynamic_cast<const OpenGLPipelineState *>( pipelineState.get() );

    //m_CurrentShader = pPipelineState->shaderProgram;

    // Set the color shader as the current shader program and set the matrices
    // that it will use for rendering.
    glUseProgram( pPipelineState->shaderProgram );

    switch ( pipelineState->cullFaceMode ) {
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

    switch ( pipelineState->depthTestMode ) {
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

        const auto &dbc = dynamic_cast<const GLDrawBatchContext &>( *pDbc );

        const MaterialData *matData = reinterpret_cast<MaterialData *>(
            pDbc->pGeom->material->gpuResource );

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

void OpenGLGraphicsManager::InitializeGeometries( const Scene &scene )
{
    // glBindBufferBase( GL_UNIFORM_BUFFER, slot, handle );
    uint32_t batch_index = 0;
    for ( const GeometryNode &node : scene.geometryNodes ) {
        for ( const Geometry &geom : node.geometries ) {
            const MeshData *drawData = reinterpret_cast<MeshData *>( geom.mesh->gpuResource );

            auto dbc = std::make_shared<GLDrawBatchContext>();
            dbc->batchIndex = batch_index++;
            dbc->vao = drawData->vao;
            dbc->mode = GL_TRIANGLES;
            dbc->type = GL_UNSIGNED_INT;
            dbc->count = drawData->count;

            dbc->pGeom = &geom;
            dbc->Model = mat4( 1 );

            m_frame.batchContexts.push_back( dbc );
        }
    }

    auto createUBO = []( int slot ) {
        GLuint handle = 0;
        glGenBuffers( 1, &handle );
        glBindBufferBase( GL_UNIFORM_BUFFER, slot, handle );
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
        return handle;
    };
    m_uboDrawFrameConstant = createUBO( 0 );
    m_uboDrawBatchConstant = createUBO( 1 );
}

void OpenGLGraphicsManager::BeginFrame( Frame &frame )
{
    GraphicsManager::BeginFrame( frame );

    SetPerFrameConstants( frame.frameContexts );
    // ImGui_ImplOpenGL3_NewFrame
}

// @TODO: make pass
extern void EditorSetup();

void OpenGLGraphicsManager::Draw()
{
    Frame &frame = m_frame;

    for ( auto &drawPass : m_drawPasses ) {
        drawPass->BeginPass( frame );
        drawPass->Draw( frame );
        drawPass->EndPass( frame );
    }
}

void OpenGLGraphicsManager::EndFrame( Frame &frame )
{
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
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
