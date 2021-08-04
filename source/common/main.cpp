#include "com_cmdline.h"
#include "com_filesystem.h"
#include "com_system.h"
#include "editor.h"
#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "main_window.h"
#include "renderer/MainRenderer.h"
#include "renderer/imgui_impl_opengl3.h"
#include "renderer/r_graphics.h"
#include "universal/core_math.h"

// include last
#include <GLFW/glfw3.h>

using namespace vct;

void updateCamera();

int main( int argc, const char** argv )
{
    bool ok = true;

    ok = ok && Com_FsInit();
    ok = ok && Com_RegisterDvars();
    ok = ok && Com_ProcessCmdLine( argc - 1, argv + 1 );
    ok = ok && Com_LoadScene();
    ok = ok && Com_ImGuiInit();
    ok = ok && MainWindow::Init();
    ok = ok && R_Init();

    EditorSetupStyle();

    ImGui_ImplGlfw_Init( MainWindow::GetRaw() );

    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    MainRenderer renderer;
    renderer.createGpuResources();

    while ( !MainWindow::ShouldClose() )
    {
        MainWindow::NewFrame();
        ImGui_ImplGlfw_NewFrame();

        // TODO: camera controller
        updateCamera();

        ivec2 size          = MainWindow::FrameSize();
        float aspect        = (float)size.x / size.y;
        Scene& scene        = Com_GetScene();
        scene.camera.aspect = aspect;

        ImGui::NewFrame();
        EditorSetup();
        ImGui::Render();

        renderer.render();
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent( backup_current_context );

        MainWindow::Present();

        scene.dirty      = false;
        scene.lightDirty = false;
    }

    renderer.destroyGpuResources();
    ImGui_ImplOpenGL3_Shutdown();

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    MainWindow::Shutdown();

    return ok ? 0 : 1;
}

static bool isKeyDown( int code )
{
    return ImGui::IsKeyDown( code );
}

void updateCamera()
{
    Scene& scene = Com_GetScene();

    constexpr float VIEW_SPEED = 2.0f;
    float CAMERA_SPEED         = 0.15f;

    if ( isKeyDown( GLFW_KEY_LEFT_SHIFT ) )
        CAMERA_SPEED *= 3.f;

    int x = isKeyDown( GLFW_KEY_D ) - isKeyDown( GLFW_KEY_A );
    int z = isKeyDown( GLFW_KEY_W ) - isKeyDown( GLFW_KEY_S );
    int y = isKeyDown( GLFW_KEY_E ) - isKeyDown( GLFW_KEY_Q );

    Camera& cam = scene.camera;
    if ( x != 0 || z != 0 )
    {
        vec3 w           = cam.direction();
        vec3 u           = glm::cross( w, vec3( 0, 1, 0 ) );
        vec3 translation = ( CAMERA_SPEED * z ) * w + ( CAMERA_SPEED * x ) * u;
        cam.position += translation;
    }

    cam.position.y += ( CAMERA_SPEED * y );

    int yaw   = isKeyDown( GLFW_KEY_RIGHT ) - isKeyDown( GLFW_KEY_LEFT );
    int pitch = isKeyDown( GLFW_KEY_UP ) - isKeyDown( GLFW_KEY_DOWN );

    if ( yaw )
        cam.yaw += VIEW_SPEED * yaw;

    if ( pitch )
    {
        cam.pitch += VIEW_SPEED * pitch;
        cam.pitch = glm::clamp( cam.pitch, -80.0f, 80.0f );
    }
}
