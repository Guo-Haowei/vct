#include "GLGraphicsManager.hpp"

#include <set>

#include "Base/Asserts.h"

#include "Core/com_dvars.h"

#include "GLPipelineStateManager.hpp"

#include "Graphics/gl_utils.h"

GraphicsManager *g_gfxMgr = new GLGraphicsManager();

static void APIENTRY debug_callback( GLenum, GLenum, unsigned int, GLenum, GLsizei, const char *, const void * );

bool GLGraphicsManager::Init()
{
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

    return ( m_initialized = true );
}

void GLGraphicsManager::Deinit()
{
    m_initialized = false;
}

void GLGraphicsManager::SetPipelineState( const std::shared_ptr<PipelineState> &pipelineState )
{
    const GLPipelineState *pPipelineState = dynamic_cast<const GLPipelineState *>( pipelineState.get() );

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
