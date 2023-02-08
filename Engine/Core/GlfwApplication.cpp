#include "GlfwApplication.hpp"

#include <GLFW/glfw3.h>

#include "com_dvars.h"
#include "imgui_impl_glfw.h"

#include "Base/Asserts.h"
#include "Base/Logger.h"
#include "Base/StringUtils.hpp"

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

    glfwGetFramebufferSize( m_pGlfwWindow, &m_config.m_width, &m_config.m_height );
    ASSERT( m_config.m_width > 0 && m_config.m_height > 0 );

    char buffer[1024];
    Sprintf( buffer,
             "%s | %s | Size: %d x %d | FSP: %.2f",
             m_config.m_appName,
             GfxBackendToString( m_config.m_gfxBackend ),
             m_config.m_width,
             m_config.m_height,
             ImGui::GetIO().Framerate );
    glfwSetWindowTitle( m_pGlfwWindow, buffer );

    BaseApplication::Tick();
}

bool GlfwApplication::CreateMainWindowInternal( bool isOpenGL )
{
    glfwSetErrorCallback( []( int code, const char* desc ) {
        LOG_FATAL( "[glfw] error(%d): %s", code, desc );
    } );

    glfwInit();

    glfwWindowHint( GLFW_DECORATED, !Dvar_GetBool( wnd_frameless ) );
    glfwWindowHint( GLFW_RESIZABLE, m_config.m_resizable );

    if ( isOpenGL ) {
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, OPENGL_DEFAULT_VERSION_MAJOR );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, OPENGL_DEFAULT_VERSION_MINOR );
        if ( Dvar_GetBool( r_debug ) ) {
            glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, 1 );
        }
    }
    else {
        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    }

    // @TODO: config size
    const GLFWvidmode* vidmode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
    const float s = 0.8f;
    ivec2 size{ s * vidmode->width, s * vidmode->height };
    // size.x = m_config.m_width;
    // size.y = m_config.m_height;

    GLFWwindow* window = glfwCreateWindow( size.x, size.y, m_config.m_appName, 0, 0 );
    if ( !window ) {
        LOG_ERROR( "glfwCreateWindow(%d, %d, %s) failed!", size.x, size.y, m_config.m_appName );
        return false;
    }

    glfwSetWindowUserPointer( window, this );

    if ( isOpenGL ) {
        glfwMakeContextCurrent( window );
        if ( !ImGui_ImplGlfw_InitForOpenGL( window, true ) ) {
            LOG_FATAL( "ImGui_ImplGlfw_InitForOpenGL() failed!" );
            return false;
        }
    }
    else {
        if ( !ImGui_ImplGlfw_InitForOther( window, true ) ) {
            LOG_FATAL( "ImGui_ImplGlfw_InitForOther() failed!" );
            return false;
        }
    }

    glfwSetFramebufferSizeCallback( window, []( GLFWwindow* window, int width, int height ) {
        auto pApp = reinterpret_cast<GlfwApplication*>( glfwGetWindowUserPointer( window ) );
        pApp->ResizeWindow( width, height );
    } );

    glfwGetFramebufferSize( window, &m_config.m_width, &m_config.m_height );
    m_pGlfwWindow = window;
    return true;
}

bool GlfwApplication::CreateMainWindow()
{
    return CreateMainWindowInternal( m_config.m_gfxBackend == GfxBackend::OpenGL );
}

bool GlfwApplication::ShouldQuit()
{
    return glfwWindowShouldClose( m_pGlfwWindow );
}

void GlfwApplication::GetFramebufferSize( int& width, int& height )
{
    width = m_config.m_width;
    height = m_config.m_height;
    ASSERT( width > 0 && height > 0 );
}

void GlfwApplication::ResizeWindow( int new_width, int new_height )
{
    ASSERT( m_pGraphicsManager );
    m_pGraphicsManager->ResizeCanvas( new_width, new_height );
}