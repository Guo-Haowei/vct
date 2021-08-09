#include "com_misc.h"

#include <filesystem>

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

#ifdef max
#undef max
#endif

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
    // validate dvars
    const int voxelTextureSize = Dvar_GetInt( r_voxelSize );
    core_assert( is_power_of_two( voxelTextureSize ) );
    core_assert( voxelTextureSize <= 256 );

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

    camera.fovy  = glm::radians( Dvar_GetFloat( cam_fov ) );
    camera.zNear = cascades[0];
    camera.zFar  = cascades[3];

    camera.yaw      = glm::radians( 180.0f );
    camera.pitch    = 0.0f;
    camera.position = Dvar_GetVec3( cam_pos );

    scene.light.color = vec3( glm::clamp( Dvar_GetFloat( light_power ), 5.0f, 30.0f ) );

    const vec3 center     = scene.boundingBox.Center();
    const vec3 size       = scene.boundingBox.Size();
    const float worldSize = glm::max( size.x, glm::max( size.y, size.z ) );
    const float texelSize = 1.0f / static_cast<float>( voxelTextureSize );
    const float voxelSize = worldSize * texelSize;

    g_perFrameCache.cache.WorldCenter   = center;
    g_perFrameCache.cache.WorldSizeHalf = 0.5f * worldSize;
    g_perFrameCache.cache.TexelSize     = texelSize;
    g_perFrameCache.cache.VoxelSize     = voxelSize;

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

    // update camera
    const ivec2 extent = MainWindow::FrameSize();
    const float aspect = (float)extent.x / extent.y;
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
        g_perFrameCache.cache.LightPVs[idx] = lightPVs[idx];
    }

    // update constants
    g_perFrameCache.cache.SunDir        = scene.light.direction;
    g_perFrameCache.cache.LightColor    = scene.light.color;
    g_perFrameCache.cache.CamPos        = camera.position;
    g_perFrameCache.cache.View          = camera.View();
    g_perFrameCache.cache.Proj          = camera.Proj();
    g_perFrameCache.cache.PV            = camera.ProjView();
    g_perFrameCache.cache.CascadedClipZ = Dvar_GetVec4( cam_cascades );
    g_perFrameCache.cache.EnableGI      = Dvar_GetBool( r_enableVXGI );
    g_perFrameCache.cache.DebugCSM      = Dvar_GetBool( r_debugCSM );
    g_perFrameCache.cache.DebugTexture  = Dvar_GetInt( r_debugTexture );
    g_perFrameCache.cache.NoTexture     = Dvar_GetBool( r_noTexture );
    g_perFrameCache.cache.ScreenWidth   = extent.x;
    g_perFrameCache.cache.ScreenHeight  = extent.y;

    // SSAO
    g_perFrameCache.cache.SSAOKernelSize   = Dvar_GetInt( r_ssaoKernelSize );
    g_perFrameCache.cache.SSAOKernelRadius = Dvar_GetFloat( r_ssaoKernelRadius );
    g_perFrameCache.cache.SSAONoiseSize    = Dvar_GetInt( r_ssaoNoiseSize );
    g_perFrameCache.cache.EnableSSAO       = Dvar_GetBool( r_enableSsao );

    // FXAA
    g_perFrameCache.cache.EnableFXAA = Dvar_GetBool( r_enableFXAA );
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