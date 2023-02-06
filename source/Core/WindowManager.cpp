#include "WindowManager.h"

#include <GLFW/glfw3.h>

#include "com_dvars.h"
#include "imgui_impl_glfw.h"

#include "Base/Asserts.h"
#include "Base/Logger.h"

static constexpr const char TITLE[] = "Editor";

WindowManager* g_wndMgr = new WindowManager();

bool WindowManager::Initialize()
{
    glfwSetErrorCallback( []( int code, const char* desc ) {
        LOG_FATAL( "[glfw] error(%d): %s", code, desc );
    } );

    glfwInit();

    glfwWindowHint( GLFW_DECORATED, !Dvar_GetBool( wnd_frameless ) );

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
    if ( Dvar_GetBool( r_debug ) ) {
        glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, 1 );
    }

    const GLFWvidmode* vidmode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
    const float s = 0.9f;
    ivec2 size{ s * vidmode->width, s * vidmode->height };

    m_window = glfwCreateWindow( size.x, size.y, TITLE, 0, 0 );
    glfwMakeContextCurrent( m_window );
    glfwGetFramebufferSize( m_window, &m_frameSize.x, &m_frameSize.y );

    ImGui_ImplGlfw_InitForOpenGL( m_window, true );

    return ( m_initialized = true );
}

void WindowManager::Finalize()
{
    ImGui_ImplGlfw_Shutdown();
    glfwDestroyWindow( m_window );
    glfwTerminate();
    m_initialized = false;
}

bool WindowManager::ShouldClose()
{
    return glfwWindowShouldClose( m_window );
}

GLFWwindow* WindowManager::GetHandle()
{
    ASSERT( m_initialized && m_window );
    return m_window;
}

void WindowManager::NewFrame()
{
    glfwPollEvents();
    ivec2& size = m_frameSize;
    glfwGetFramebufferSize( m_window, &size.x, &size.y );

    // mouse position
    {
        double x, y;
        glfwGetCursorPos( m_window, &x, &y );
        m_mousePos.x = static_cast<float>( x );
        m_mousePos.y = static_cast<float>( y );
    }

    // title
    char buffer[1024];
    snprintf( buffer, sizeof( buffer ),
              "%s | Size: %d x %d | Mouse: %d x %d | FPS: %.1f",
              TITLE,
              size.x, size.y,
              int( m_mousePos.x ), int( m_mousePos.y ),
              ImGui::GetIO().Framerate );
    glfwSetWindowTitle( m_window, buffer );

    ImGui_ImplGlfw_NewFrame();
}

ivec2 WindowManager::FrameSize()
{
    ASSERT( m_initialized && m_window );
    return m_frameSize;
}

vec2 WindowManager::MousePos()
{
    ASSERT( m_initialized && m_window );
    return m_mousePos;
}

bool WindowManager::IsKeyDown( ImGuiKey code )
{
    return ImGui::IsKeyDown( code );
}

bool WindowManager::IsMouseInScreen()
{
    bool inside = true;
    inside &= m_mousePos.x >= 0;
    inside &= m_mousePos.y >= 0;
    inside &= m_mousePos.x <= m_frameSize.x;
    inside &= m_mousePos.y <= m_frameSize.y;
    return inside;
}
