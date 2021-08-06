#include "com_system.h"

#include <filesystem>

#include "Globals.h"
#include "com_filesystem.h"
#include "imgui/imgui.h"
#include "scene/SceneLoader.h"
#include "universal/core_assert.h"
#include "universal/print.h"

#define DEFINE_DVAR
#include "com_dvars.h"

using namespace vct;

static vct::Scene g_scene;

bool Com_RegisterDvars()
{
#define REGISTER_DVAR
#include "com_dvars.h"
    return true;
}

bool Com_LoadScene()
{
    SceneLoader loader;
    vct::Scene& scene = g_scene;

    const float worldScale = Dvar_GetFloat( scene_scale );
    const mat4 S           = glm::scale( mat4( 1 ), vec3( worldScale ) );
    const mat4 trans       = S;
    const char* scenePath  = Dvar_GetString( scene );
    loader.loadGltf( scenePath, scene, trans );

    // TODO: configure the rest
    Camera& camera = scene.camera;

    camera.fovy     = glm::radians( 60.0f );
    camera.aspect   = 1.0f;
    camera.zNear    = 0.2f;
    camera.zFar     = 100.0f;
    camera.yaw      = glm::radians( 180.0f );
    camera.pitch    = -0.04f;
    camera.position = Dvar_GetVec3( cam_pos );

    scene.light.position = vec3( -.5f, 25.f, -.5f );
    scene.light.color    = vec3( 15.0f );

    const vec3 center     = scene.boundingBox.Center();
    vec3 size             = scene.boundingBox.Size();
    constexpr float scale = .6f;

    size.x *= scale;
    size.z *= scale;
    scene.shadowBox.FromCenterSize( center, size );

    Com_PrintSuccess( "Scene '%s' loaded", scenePath );
    return true;
}

vct::Scene& Com_GetScene()
{
    return g_scene;
}

bool Com_ImGuiInit()
{
    constexpr char* kDefaultIniFileName   = "imgui.ini";
    constexpr char kDefaultEditorLayout[] = "default/imgui.ini";
    static char s_iniFileNameLoad[kMaxOSPath];

    if ( std::filesystem::exists( kDefaultIniFileName ) )
    {
        strncpy( s_iniFileNameLoad, kDefaultIniFileName, sizeof( s_iniFileNameLoad ) );
    }
    else
    {
        Com_FsBuildPath( s_iniFileNameLoad, kMaxOSPath, kDefaultEditorLayout );
    }

    Com_Printf( "[imgui] loading imgui config from '%s'", s_iniFileNameLoad );
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io        = ImGui::GetIO();
    io.IniFilenameLoad = s_iniFileNameLoad;
    io.IniFilenameSave = kDefaultIniFileName;
    io.FontGlobalScale = 1.5f;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport / Platform Windows

    Com_PrintSuccess( "ImGui initialized" );
    return true;
}

void Com_ImGuiShutdown()
{
}
