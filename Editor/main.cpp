#include "imgui/imgui.h"

#include "Engine/Base/Asserts.h"
#include "Engine/Base/Logger.h"

#include "Engine/Core/dvar_api.h"
#include "Engine/Core/com_misc.h"
#include "Engine/Core/editor.h"
#include "Engine/Core/WindowManager.h"

#include "Engine/Manager/BaseApplication.hpp"
#include "Engine/Manager/AssetLoader.hpp"
#include "Engine/Interface/IGraphicsManager.hpp"

#include "Engine/RHI/OpenGLPipelineStateManager.hpp"

int main( int argc, const char** argv )
{
    OpenGLPipelineStateManager pipelineStateManager;

    BaseApplication app;

    if ( !app.ProcessCommandLineParameters( argc, argv ) ) {
        return false;
    }

    app.RegisterManagerModule( g_fileMgr );
    app.RegisterManagerModule( g_gfxMgr );
    app.RegisterManagerModule( &pipelineStateManager );
    if ( !app.Initialize() ) {
        return -1;
    }

    while ( !g_wndMgr->ShouldClose() ) {
        g_wndMgr->NewFrame();

        Com_UpdateWorld();
        EditorSetup();

        g_gfxMgr->Tick();

        Com_GetScene().dirty = false;
    }

    pipelineStateManager.Finalize();

    g_gfxMgr->Finalize();
    manager_deinit( g_wndMgr );
    ImGui::DestroyContext();

    g_fileMgr->Finalize();

    return 0;
}