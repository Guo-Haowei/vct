#include "com_cmdline.h"
#include "com_filesystem.h"
#include "com_misc.h"
#include "editor.h"
#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "main_window.h"
#include "renderer/MainRenderer.h"
#include "renderer/imgui_impl_opengl3.h"
#include "renderer/r_graphics.h"
#include "universal/core_math.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace vct;

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

        Com_UpdateWorld();

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
    }

    renderer.destroyGpuResources();
    ImGui_ImplOpenGL3_Shutdown();

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    MainWindow::Shutdown();

    return ok ? 0 : 1;
}
