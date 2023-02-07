#include "Engine/Base/Asserts.h"
#include "Engine/Base/Logger.h"

#include "Engine/Core/com_dvars.h"
#include "Engine/Core/GlfwApplication.hpp"

#include "Engine/Manager/AssetLoader.hpp"
#include "Engine/Manager/BaseApplication.hpp"
#include "Engine/Manager/SceneManager.hpp"
#include "Engine/Interface/IGraphicsManager.hpp"

#include "Engine/RHI/OpenGLPipelineStateManager.hpp"

#include "imgui/imgui.h"

#include "EditorLogic.hpp"

int main( int argc, const char** argv )
{
    AssetLoader assetLoader;
    SceneManager sceneManager;
    OpenGLPipelineStateManager pipelineStateManager;
    EditorLogic logic;

    GlfwApplication app;

    app.RegisterManagerModule( &assetLoader );
    app.RegisterManagerModule( &sceneManager );
    app.RegisterManagerModule( g_gfxMgr );
    app.RegisterManagerModule( &pipelineStateManager );
    app.RegisterManagerModule( &logic );

    // Initialize Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    [[maybe_unused]] ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    if ( !app.ProcessCommandLine( argc, argv ) ) {
        return -1;
    }

    // @TODO: stream load
    if ( !sceneManager.LoadScene( Dvar_GetString( scene ) ) ) {
        return -1;
    }

    if ( !app.CreateMainWindow() ) {
        return -1;
    }

    if ( !app.Initialize() ) {
        return -1;
    }

    while ( !app.ShouldQuit() ) {
        app.Tick();

        // @TODO: remove
        Com_GetScene().dirty = false;
    }

    app.Finalize();
    ImGui::DestroyContext();

    return 0;
}