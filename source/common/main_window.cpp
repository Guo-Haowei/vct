#include "main_window.h"

#include <GLFW/glfw3.h>

#include "com_dvars.h"
#include "imgui/imgui.h"
#include "universal/core_assert.h"
#include "universal/print.h"

namespace MainWindow {

static bool g_initialized;
static GLFWwindow* g_window;
static ivec2 g_frameSize;
static vec2 g_mousePos;

static constexpr const char TITLE[] = "Editor";

static constexpr ivec2 MAX_FRAME_SIZE = vec2( 2560, 1440 );

bool Init()
{
    core_assert( !g_initialized );

    glfwSetErrorCallback( []( int code, const char* desc ) {
        Com_PrintFatal( "[glfw] error(%d): %s", code, desc );
    } );

    glfwInit();

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
    if ( Dvar_GetBool( r_debug ) )
    {
        glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, 1 );
    }

    const GLFWvidmode* vidmode = glfwGetVideoMode( glfwGetPrimaryMonitor() );

    const float scale = 0.8f;
    ivec2 extents;
    extents.x = static_cast<int>( scale * vidmode->width );
    extents.y = static_cast<int>( scale * vidmode->height );
    extents   = glm::min( extents, MAX_FRAME_SIZE );

    g_window = glfwCreateWindow( extents.x, extents.y, TITLE, 0, 0 );
    core_assert( g_window );

    glfwMakeContextCurrent( g_window );

    Com_PrintSuccess( "MainWindow created %d x %d", extents.x, extents.y );
    glfwGetFramebufferSize( g_window, &g_frameSize.x, &g_frameSize.y );
    return g_initialized = true;
}

bool ShouldClose()
{
    return glfwWindowShouldClose( g_window );
}

void Shutdown()
{
    glfwDestroyWindow( g_window );
    glfwTerminate();
}

GLFWwindow* GetRaw()
{
    core_assert( g_initialized && g_window );
    return g_window;
}

void NewFrame()
{
    glfwPollEvents();
    ivec2& size = g_frameSize;
    glfwGetFramebufferSize( g_window, &size.x, &size.y );

    // mouse position
    {
        double x, y;
        glfwGetCursorPos( g_window, &x, &y );
        g_mousePos.x = static_cast<float>( x );
        g_mousePos.y = static_cast<float>( y );
    }

    // title
    char buffer[1024];
    snprintf( buffer, sizeof( buffer ),
              "%s | Size: %d x %d | Mouse: %d x %d | FPS: %.1f",
              TITLE,
              size.x, size.y,
              int( g_mousePos.x ), int( g_mousePos.y ),
              ImGui::GetIO().Framerate );
    glfwSetWindowTitle( g_window, buffer );
}

ivec2 FrameSize()
{
    return g_frameSize;
}

void Present()
{
    glfwSwapBuffers( g_window );
}

vec2 MousePos()
{
    return g_mousePos;
}

bool IsKeyDown( int code )
{
    return ImGui::IsKeyDown( code );
}

bool IsMouseInScreen()
{
    bool inside = true;
    inside &= g_mousePos.x >= 0;
    inside &= g_mousePos.y >= 0;
    inside &= g_mousePos.x <= g_frameSize.x;
    inside &= g_mousePos.y <= g_frameSize.y;
    return inside;
}

}  // namespace MainWindow
