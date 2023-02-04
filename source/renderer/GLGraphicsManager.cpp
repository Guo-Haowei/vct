#include "GLGraphicsManager.h"

#include <set>

#include "gl_utils.h"

#include "Core/com_dvars.h"

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
