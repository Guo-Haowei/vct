#include "main_window.h"

#include <GLFW/glfw3.h>

#include "com_dvars.h"
#include "universal/core_assert.h"
#include "universal/print.h"

namespace MainWindow {

static bool g_initialized;
static GLFWwindow* g_window;
static ivec2 g_frameSize;

static constexpr const char TITLE[] = "Editor";

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

    const float scale = 0.7f;
    const int width   = static_cast<int>( scale * vidmode->width );
    const int height  = static_cast<int>( scale * vidmode->height );

    g_window = glfwCreateWindow( width, height, TITLE, 0, 0 );
    core_assert( g_window );

    glfwMakeContextCurrent( g_window );

    Com_PrintSuccess( "MainWindow created %d x %d", width, height );
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

    char buffer[1024];
    snprintf( buffer, sizeof( buffer ), "%s | %dx%d (OpenGL)", TITLE, size.x, size.y );
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

}  // namespace MainWindow
