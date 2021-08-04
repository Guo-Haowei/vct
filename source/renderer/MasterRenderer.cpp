#include "MasterRenderer.h"

#include <iostream>

#include "GLPrerequisites.h"
#include "ImguiRenderer.h"
#include "common/Window.h"
#include "common/com_dvars.h"
#include "universal/dvar_api.h"
#include "universal/print.h"

using std::cout;
using std::endl;

/// TODO: remove
#include "imgui_impl_opengl3.h"
// #include "imgui/imgui.h"

namespace vct {

static void APIENTRY glDebugOutput(
    GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char *message,
    const void *userParam );

void MasterRenderer::initialize( Window *pWindow )
{
    m_pWindow = pWindow;

    if ( gladLoadGL() == 0 )
    {
        Com_PrintFatal( "[glad] failed to load gl functions" );
    }

    Com_Printf( "[opengl] renderer: %s", glGetString( GL_RENDERER ) );
    Com_Printf( "[opengl] version: %s", glGetString( GL_VERSION ) );

    if ( Dvar_GetBool( r_debug ) )
    {
        int flags;
        glGetIntegerv( GL_CONTEXT_FLAGS, &flags );
        if ( flags & GL_CONTEXT_FLAG_DEBUG_BIT )
        {
            Com_Printf( "[opengl] debug callback enabled" );
            glEnable( GL_DEBUG_OUTPUT );
            glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
            glDebugMessageCallback( glDebugOutput, nullptr );
            glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
        }
    }

    /// initialize renderers
    m_mainRenderer.setWindow( m_pWindow );
    m_mainRenderer.createGpuResources();

    m_imguiRenderer.createGpuResources();
}

void MasterRenderer::update()
{
    m_mainRenderer.render();
    m_imguiRenderer.render();
}

void MasterRenderer::finalize()
{
    m_mainRenderer.destroyGpuResources();
    m_imguiRenderer.destroyGpuResources();
}

static void APIENTRY glDebugOutput(
    GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char *message,
    const void *userParam )
{
    // ignore non-significant error/warning codes
    if ( id == 131185 ||
         // GL_STREAM_DRAW will use VIDEO memory as the source for buffer object operations.
         id == 131204
         // The texture object (0) bound to texture image unit 0 does not have a defined base level and cannot be used for texture mapping.
    )
    {
        return;
    }

    bool minorError = false;
    if ( id == 131169 || id == 131218 )
    {
        minorError = true;
    }

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch ( source )
    {
        case GL_DEBUG_SOURCE_API: std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER: std::cout << "Source: Other"; break;
    }
    std::cout << std::endl;

    switch ( type )
    {
        case GL_DEBUG_TYPE_ERROR: std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY: std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER: std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP: std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER: std::cout << "Type: Other"; break;
    }
    std::cout << std::endl;

    switch ( severity )
    {
        case GL_DEBUG_SEVERITY_HIGH: std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW: std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    }
    std::cout << std::endl;
    std::cout << std::endl;

    if ( !minorError )
        __debugbreak();
}

}  // namespace vct
