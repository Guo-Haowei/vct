#include "FileManager.h"
#include "com_misc.h"
#include "editor.h"
#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "universal/core_math.h"
#include "universal/dvar_api.h"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Core/FileManager.h"
#include "Core/WindowManager.h"

#include "Graphics/MainRenderer.h"
#include "Graphics/imgui_impl_opengl3.h"
#include "Graphics/GraphicsManager.hpp"
#include "Graphics/PipelineStateManager.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace vct;

static int app_main( int argc, const char** argv )
{
    bool ok = true;

    ok = ok && Com_RegisterDvars();
    ok = ok && dvar_process_from_cmdline( argc - 1, argv + 1 );

    ok = ok && manager_init( g_fileMgr );

    ok = ok && Com_LoadScene();
    ok = ok && Com_ImGuiInit();

    ok = ok && manager_init( g_wndMgr );
    ok = ok && manager_init( g_gfxMgr );

    EditorSetupStyle();

    ImGui_ImplGlfw_Init( g_wndMgr->GetHandle() );

    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    MainRenderer renderer;

    // TODO: refactor
    ok = ok && g_pPipelineStateManager->Init();
    renderer.createGpuResources();

    g_gfxMgr->InitializeGeometries( Com_GetScene() );

    while ( !g_wndMgr->ShouldClose() ) {
        g_wndMgr->NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        EditorSetup();
        ImGui::Render();

        Com_UpdateWorld();

        g_gfxMgr->UpdateConstants();  // update constants

        Frame dummy;
        g_gfxMgr->BeginFrame( dummy );  // upload constants

        renderer.render();
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent( backup_current_context );

        g_wndMgr->Present();

        Com_GetScene().dirty = false;
    }

    g_pPipelineStateManager->Deinit();

    renderer.destroyGpuResources();
    ImGui_ImplOpenGL3_Shutdown();

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    manager_deinit( g_gfxMgr );
    manager_deinit( g_wndMgr );
    manager_deinit( g_fileMgr );

    return ok ? 0 : 1;
}

int main( int argc, const char** argv )
{
    return app_main( argc, argv );
}
