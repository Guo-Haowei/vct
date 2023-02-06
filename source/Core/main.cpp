#include "FileManager.h"
#include "com_misc.h"
#include "editor.h"
#include "imgui/imgui.h"
#include "universal/dvar_api.h"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Core/FileManager.h"
#include "Core/WindowManager.h"

#include "Graphics/MainRenderer.h"
#include "Graphics/GraphicsManager.hpp"
#include "Graphics/PipelineStateManager.hpp"

#include "RHI/imgui_impl_opengl3.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

static int app_main( int argc, const char** argv )
{
    bool ok = true;

    ok = ok && Com_RegisterDvars();
    ok = ok && dvar_process_from_cmdline( argc - 1, argv + 1 );

    ok = ok && manager_init( g_fileMgr );

    ok = ok && Com_LoadScene();
    ok = ok && Com_ImGuiInit();

    ok = ok && manager_init( g_wndMgr );
    ok = ok && g_gfxMgr->Initialize();

    // TODO: refactor
    ok = ok && g_pPipelineStateManager->Initialize();
    renderer.createGpuResources();

    g_gfxMgr->InitializeGeometries( Com_GetScene() );

    while ( !g_wndMgr->ShouldClose() ) {
        g_wndMgr->NewFrame();

        Com_UpdateWorld();
        EditorSetup();

        g_gfxMgr->Tick();

        Com_GetScene().dirty = false;
    }

    g_pPipelineStateManager->Finalize();

    renderer.destroyGpuResources();
    g_gfxMgr->Finalize();
    manager_deinit( g_wndMgr );
    ImGui::DestroyContext();

    manager_deinit( g_fileMgr );

    return ok ? 0 : 1;
}

int main( int argc, const char** argv )
{
    return app_main( argc, argv );
}
