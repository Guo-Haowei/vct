#include "Window.h"

#include "imgui/imgui.h"
#include "universal/core_assert.h"
#include "universal/print.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <string>
using std::string;
using std::to_string;

namespace vct {

void Window::initialize()
{
    glfwSetErrorCallback( []( int code, const char* desc ) {
        Com_PrintFatal( "[glfw] error(%d): %s", code, desc );
    } );

    glfwInit();

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
    IF_TEST( glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, 1 ) );

    const GLFWvidmode* vidmode = glfwGetVideoMode( glfwGetPrimaryMonitor() );

    const float scale = 0.9f;
    const int width   = static_cast<int>( scale * vidmode->width );
    const int height  = static_cast<int>( scale * vidmode->height );

    m_pGlfwWindow = glfwCreateWindow( width, height, "Voxel Cone Tracing", 0, 0 );

    core_assert( m_pGlfwWindow );

    glfwMakeContextCurrent( m_pGlfwWindow );
    glfwSwapInterval( 1 );  // enable vsync

    /// init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;  // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;   // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "windows_glfw";

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab]         = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow]   = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow]  = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow]     = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow]   = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp]      = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown]    = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home]        = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End]         = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert]      = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete]      = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace]   = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space]       = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter]       = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape]      = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A]           = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C]           = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V]           = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X]           = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y]           = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z]           = GLFW_KEY_Z;

    // cursors
    GLFWerrorfun prev_error_callback            = glfwSetErrorCallback( NULL );
    m_mouseCursors[ImGuiMouseCursor_Arrow]      = glfwCreateStandardCursor( GLFW_ARROW_CURSOR );
    m_mouseCursors[ImGuiMouseCursor_TextInput]  = glfwCreateStandardCursor( GLFW_IBEAM_CURSOR );
    m_mouseCursors[ImGuiMouseCursor_ResizeNS]   = glfwCreateStandardCursor( GLFW_VRESIZE_CURSOR );
    m_mouseCursors[ImGuiMouseCursor_ResizeEW]   = glfwCreateStandardCursor( GLFW_HRESIZE_CURSOR );
    m_mouseCursors[ImGuiMouseCursor_Hand]       = glfwCreateStandardCursor( GLFW_HAND_CURSOR );
    m_mouseCursors[ImGuiMouseCursor_ResizeAll]  = glfwCreateStandardCursor( GLFW_RESIZE_ALL_CURSOR );
    m_mouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor( GLFW_RESIZE_NESW_CURSOR );
    m_mouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor( GLFW_RESIZE_NWSE_CURSOR );
    m_mouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor( GLFW_NOT_ALLOWED_CURSOR );
    glfwSetErrorCallback( prev_error_callback );

    io.GetClipboardTextFn = []( void* userData ) {
        return glfwGetClipboardString( (GLFWwindow*)userData );
    };
    io.SetClipboardTextFn = []( void* userData, const char* text ) {
        return glfwSetClipboardString( (GLFWwindow*)userData, text );
    };
    io.ClipboardUserData = m_pGlfwWindow;
    io.ImeWindowHandle   = (void*)glfwGetWin32Window( m_pGlfwWindow );

    /// callbacks
    glfwSetWindowUserPointer( m_pGlfwWindow, this );
    glfwSetScrollCallback( m_pGlfwWindow, scrollCallback );
    glfwSetKeyCallback( m_pGlfwWindow, keyCallback );
    glfwSetCharCallback( m_pGlfwWindow, charCallback );

    m_time = 0.0;
}

void Window::finalize()
{
    for ( GLFWcursor* cursor : m_mouseCursors )
        glfwDestroyCursor( cursor );

    glfwDestroyWindow( m_pGlfwWindow );
    glfwTerminate();
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose( m_pGlfwWindow );
}

void Window::pollEvents()
{
    glfwPollEvents();
}

void Window::swapBuffers()
{
    glfwSwapBuffers( m_pGlfwWindow );
}

void Window::getWindowExtent( int* width, int* height ) const
{
    glfwGetWindowSize( m_pGlfwWindow, width, height );
}

void Window::getFrameExtent( int* width, int* height ) const
{
    glfwGetFramebufferSize( m_pGlfwWindow, width, height );
}

void Window::beginFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    // TODO: remove assert in release build
    core_assert( io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame()." );

    int w, h;
    int display_w, display_h;
    glfwGetWindowSize( m_pGlfwWindow, &w, &h );
    glfwGetFramebufferSize( m_pGlfwWindow, &display_w, &display_h );
    io.DisplaySize = ImVec2( (float)w, (float)h );
    if ( w > 0 && h > 0 )
        io.DisplayFramebufferScale = ImVec2( (float)display_w / w, (float)display_h / h );

    // Setup time step
    double current_time = glfwGetTime();
    io.DeltaTime        = m_time > 0.0 ? (float)( current_time - m_time ) : (float)( 1.0f / 60.0f );
    m_time              = current_time;

    /// update mouse position and buttons
    for ( int i = 0; i < m_mouseJustPressed.size(); ++i )
    {
        io.MouseDown[i]       = m_mouseJustPressed[i] || glfwGetMouseButton( m_pGlfwWindow, i ) != 0;
        m_mouseJustPressed[i] = false;
    }

    const ImVec2 mouse_pos_backup = io.MousePos;
    io.MousePos                   = ImVec2( -FLT_MAX, -FLT_MAX );
    const bool focused            = glfwGetWindowAttrib( m_pGlfwWindow, GLFW_FOCUSED ) != 0;
    if ( focused )
    {
        if ( io.WantSetMousePos )
            glfwSetCursorPos( m_pGlfwWindow, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y );
        else
        {
            double mouse_x, mouse_y;
            glfwGetCursorPos( m_pGlfwWindow, &mouse_x, &mouse_y );
            io.MousePos = ImVec2( (float)mouse_x, (float)mouse_y );
        }
    }

    /// update mouse cursor
    if ( ( io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange ) || glfwGetInputMode( m_pGlfwWindow, GLFW_CURSOR ) == GLFW_CURSOR_DISABLED )
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if ( imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor )
        glfwSetInputMode( m_pGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
    else
    {
        glfwSetCursor( m_pGlfwWindow, m_mouseCursors[imgui_cursor] ? m_mouseCursors[imgui_cursor] : m_mouseCursors[ImGuiMouseCursor_Arrow] );
        glfwSetInputMode( m_pGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
    }
}

/// callbacks
void Window::scrollCallback( GLFWwindow* window, double xoffset, double yoffset )
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}

void Window::keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    ImGuiIO& io = ImGui::GetIO();
    if ( action == GLFW_PRESS )
        io.KeysDown[key] = true;
    if ( action == GLFW_RELEASE )
        io.KeysDown[key] = false;

    // Modifiers are not reliable across systems
    io.KeyCtrl  = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt   = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = false;
}

void Window::mouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
{
    Window* pWindow = (Window*)glfwGetWindowUserPointer( window );
    if ( action == GLFW_PRESS && button >= 0 && button < pWindow->m_mouseJustPressed.size() )
    {
        pWindow->m_mouseJustPressed[button] = true;
    }
}

void Window::charCallback( GLFWwindow* window, unsigned int c )
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter( c );
}

bool Window::isKeyDown( int key ) const
{
    ImGuiIO& io = ImGui::GetIO();
    return io.KeysDown[key];
}

}  // namespace vct
