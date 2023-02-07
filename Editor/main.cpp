#include "Engine/Base/Asserts.h"
#include "Engine/Base/Logger.h"

#include "Engine/Core/com_dvars.h"
#include "Engine/Core/editor.h"
#include "Engine/Core/WindowManager.h"

#include "Engine/Manager/AssetLoader.hpp"
#include "Engine/Manager/BaseApplication.hpp"
#include "Engine/Manager/SceneManager.hpp"
#include "Engine/Interface/IGraphicsManager.hpp"

#include "Engine/RHI/OpenGLPipelineStateManager.hpp"

#include "imgui/imgui.h"

int main( int argc, const char** argv )
{
    OpenGLPipelineStateManager pipelineStateManager;
    AssetLoader assetLoader;
    SceneManager sceneManager;

    BaseApplication app;

    app.RegisterManagerModule( &assetLoader );
    app.RegisterManagerModule( &sceneManager );
    app.RegisterManagerModule( g_gfxMgr );
    app.RegisterManagerModule( &pipelineStateManager );

    // Initialize Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    [[maybe_unused]] ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    if ( !app.Initialize( argc, argv ) ) {
        return -1;
    }

    while ( !g_wndMgr->ShouldClose() ) {
        g_wndMgr->NewFrame();

        Com_UpdateWorld();
        EditorSetup();

        g_gfxMgr->Tick();

        Com_GetScene().dirty = false;
    }

    app.Finalize();

    manager_deinit( g_wndMgr );

    ImGui::DestroyContext();

    return 0;
}