#include "com_misc.h"

#include <filesystem>

#include "Globals.h"
#include "com_filesystem.h"
#include "imgui/imgui.h"
#include "main_window.h"
#include "renderer/r_cbuffers.h"
#include "renderer/r_sun_shadow.h"
#include "scene/SceneLoader.h"
#include "universal/core_assert.h"
#include "universal/print.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define DEFINE_DVAR
#include "com_dvars.h"

static Scene g_scene;

static void ControlCamera( Camera& camera );

bool Com_RegisterDvars()
{
#define REGISTER_DVAR
#include "com_dvars.h"
    return true;
}

bool Com_LoadScene()
{
    vct::SceneLoader loader;
    Scene& scene = g_scene;

    const float worldScale = Dvar_GetFloat( scene_scale );
    const mat4 S           = glm::scale( mat4( 1 ), vec3( worldScale ) );
    const mat4 trans       = S;
    const char* scenePath  = Dvar_GetString( scene );
    loader.loadGltf( scenePath, scene, trans );

    // TODO: configure the rest
    Camera& camera = scene.camera;

    const vec4 cascades = Dvar_GetVec4( cam_cascades );

    // TODO: config
    camera.fovy  = glm::radians( 60.0f );
    camera.zNear = cascades[0];
    camera.zFar  = cascades[3];

    camera.yaw      = glm::radians( 180.0f );
    camera.pitch    = 0.0f;
    camera.position = Dvar_GetVec3( cam_pos );

    scene.light.direction = glm::normalize( vec3( 4, 25, 4 ) );
    scene.light.color     = vec3( 15.0f );

    const vec3 center     = scene.boundingBox.Center();
    vec3 size             = scene.boundingBox.Size();
    constexpr float scale = .6f;

    size.x *= scale;
    size.z *= scale;
    scene.shadowBox.FromCenterSize( center, size );

    Com_PrintSuccess( "Scene '%s' loaded", scenePath );
    return true;
}

Scene& Com_GetScene()
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

void Com_UpdateWorld()
{
    Scene& scene = Com_GetScene();

    static bool firstFrame = true;
    if ( firstFrame )
    {
        firstFrame = false;
    }
    else
    {
        scene.dirty = false;
    }

    // update camera
    const ivec2 size   = MainWindow::FrameSize();
    const float aspect = (float)size.x / size.y;
    core_assert( aspect > 0.0f );

    Camera& camera = scene.camera;
    ControlCamera( camera );
    camera.SetAspect( aspect );
    camera.UpdatePV();

    // update lightspace matrices
    mat4 lightPVs[NUM_CASCADES];
    R_LightSpaceMatrix( camera, scene.light.direction, lightPVs );

    for ( size_t idx = 0; idx < array_length( lightPVs ); ++idx )
    {
        g_perframeCache.cache.LightPVs[idx] = lightPVs[idx];
    }

    // update constants
    g_perframeCache.cache.SunDir        = scene.light.direction;
    g_perframeCache.cache.CamPos        = camera.position;
    g_perframeCache.cache.View          = camera.View();
    g_perframeCache.cache.PV            = camera.ProjView();
    const vec4 cascadedClipZ            = Dvar_GetVec4( cam_cascades );
    g_perframeCache.cache.CascadedClipZ = cascadedClipZ;
}

static void ControlCamera( Camera& camera )
{
    constexpr float VIEW_SPEED = 2.0f;
    float CAMERA_SPEED         = 0.15f;

    if ( ImGui::IsKeyDown( GLFW_KEY_LEFT_SHIFT ) )
        CAMERA_SPEED *= 3.f;

    int x = ImGui::IsKeyDown( GLFW_KEY_D ) - ImGui::IsKeyDown( GLFW_KEY_A );
    int z = ImGui::IsKeyDown( GLFW_KEY_W ) - ImGui::IsKeyDown( GLFW_KEY_S );
    int y = ImGui::IsKeyDown( GLFW_KEY_E ) - ImGui::IsKeyDown( GLFW_KEY_Q );

    if ( x != 0 || z != 0 )
    {
        vec3 w           = camera.direction();
        vec3 u           = glm::cross( w, vec3( 0, 1, 0 ) );
        vec3 translation = ( CAMERA_SPEED * z ) * w + ( CAMERA_SPEED * x ) * u;
        camera.position += translation;
    }

    camera.position.y += ( CAMERA_SPEED * y );

    int yaw   = ImGui::IsKeyDown( GLFW_KEY_RIGHT ) - ImGui::IsKeyDown( GLFW_KEY_LEFT );
    int pitch = ImGui::IsKeyDown( GLFW_KEY_UP ) - ImGui::IsKeyDown( GLFW_KEY_DOWN );

    if ( yaw )
    {
        camera.yaw += VIEW_SPEED * yaw;
    }

    if ( pitch )
    {
        camera.pitch += VIEW_SPEED * pitch;
        camera.pitch = glm::clamp( camera.pitch, -80.0f, 80.0f );
    }
}