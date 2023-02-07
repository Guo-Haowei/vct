#include "Engine/Base/Asserts.h"
#include "Engine/Base/Logger.h"

#include "Engine/Core/com_dvars.h"
#include "Engine/Core/GlfwApplication.hpp"

#include "Engine/Manager/AssetLoader.hpp"
#include "Engine/Manager/BaseApplication.hpp"
#include "Engine/Manager/SceneManager.hpp"
#include "Engine/Interface/IGraphicsManager.hpp"

#include "Engine/RHI/OpenGL/OpenGLGraphicsManager.hpp"
#include "Engine/RHI/OpenGL/OpenGLPipelineStateManager.hpp"

#include "Engine/RHI/D3d11/D3d11GraphicsManager.hpp"
#include "Engine/RHI/D3d11/D3d11PipelineStateManager.hpp"

#define USE_DX11
#include "Engine/RHI/ConfigBackend.hpp"

#include "imgui/imgui.h"

#include "EditorLogic.hpp"

int main( int argc, const char** argv )
{
    const GfxBackend backend = USING( DX11_PROGRAM ) ? GfxBackend::D3d11 : GfxBackend::OpenGL;
    Configuration config( backend, "Editor", 800, 600 );
    GlfwApplication app( config );

    AssetLoader assetLoader;
    SceneManager sceneManager;
    TGraphicsManager graphicsManager;
    TPipelineStateManager pipelineStateManager;
    EditorLogic logic;

    app.RegisterManagerModule( &assetLoader );
    app.RegisterManagerModule( &sceneManager );
    app.RegisterManagerModule( &graphicsManager );
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