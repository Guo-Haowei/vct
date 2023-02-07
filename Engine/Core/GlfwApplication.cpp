#include "GlfwApplication.hpp"

#include <GLFW/glfw3.h>

#include "com_dvars.h"
#include "imgui_impl_glfw.h"

#include "Base/Asserts.h"
#include "Base/Logger.h"

void GlfwApplication::Finalize()
{
    BaseApplication::Finalize();

    if ( ImGui::GetCurrentContext() ) {
        ImGui_ImplGlfw_Shutdown();
    }

    glfwDestroyWindow( m_pGlfwWindow );
    glfwTerminate();
}

void GlfwApplication::Tick()
{
    glfwPollEvents();

    ImGui_ImplGlfw_NewFrame();

    glfwGetFramebufferSize( m_pGlfwWindow, &m_width, &m_height );
    assert( m_width > 0 && m_height > 0 );

    char buffer[1024]{ 0 };
    snprintf( buffer, sizeof( buffer ), "%s | Size: %d x %d", m_title.c_str(), m_width, m_height );
    glfwSetWindowTitle( m_pGlfwWindow, buffer );

    BaseApplication::Tick();
}

bool GlfwApplication::CreateMainWindow()
{
    glfwSetErrorCallback( []( int code, const char* desc ) {
        LOG_FATAL( "[glfw] error(%d): %s", code, desc );
    } );

    glfwInit();

    glfwWindowHint( GLFW_DECORATED, !Dvar_GetBool( wnd_frameless ) );

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, OPENGL_DEFAULT_VERSION_MAJOR );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, OPENGL_DEFAULT_VERSION_MINOR );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
    if ( Dvar_GetBool( r_debug ) ) {
        glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, 1 );
    }

    const GLFWvidmode* vidmode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
    const float s = 0.9f;
    ivec2 size{ s * vidmode->width, s * vidmode->height };

    m_title = "Editor";
    GLFWwindow* window = glfwCreateWindow( size.x, size.y, m_title.c_str(), 0, 0 );
    glfwMakeContextCurrent( window );

    glfwGetFramebufferSize( window, &m_width, &m_height );

    ImGui_ImplGlfw_InitForOpenGL( window, true );
    m_pGlfwWindow = window;

    return true;
}

bool GlfwApplication::ShouldQuit()
{
    return glfwWindowShouldClose( m_pGlfwWindow );
}

void GlfwApplication::GetFramebufferSize( uint32_t& width, uint32_t& height )
{
    assert( m_width > 0 && m_height > 0 );
    width = m_width;
    height = m_height;
}
