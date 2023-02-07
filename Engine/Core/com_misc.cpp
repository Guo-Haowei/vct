#include "com_misc.h"

#include <filesystem>

#include "imgui/imgui.h"
#include "WindowManager.h"
#include "AssimpLoader.hpp"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Graphics/r_cbuffers.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "com_dvars.h"

#ifdef max
#undef max
#endif

static Scene g_scene;

static void ControlCamera( Camera& camera );

bool Com_LoadScene()
{
    AssimpLoader loader;
    Scene& scene = g_scene;

    const char* scenePath = Dvar_GetString( scene );

    if ( !scenePath[0] ) {
        LOG_FATAL( "Scene not specified, set it by +set scene <name> or +exec <lua-file>" );
        return false;
    }

    scene.m_root = scene.RegisterEntity( "world", Entity::FLAG_NONE );
    scene.m_root->m_trans = glm::scale( vec3( Dvar_GetFloat( scene_scale ) ) );

    loader.loadGltf( scenePath, scene );
    scene.m_aabb.ApplyMatrix( scene.m_root->m_trans );

    Camera& camera = scene.camera;

    camera.fovy = glm::radians( Dvar_GetFloat( cam_fov ) );
    camera.zNear = 1.f;
    camera.zFar = 1000.f;

    camera.yaw = glm::radians( 180.0f );
    camera.pitch = 0.0f;
    camera.position = Dvar_GetVec3( cam_pos );

    scene.light.color = vec3( glm::clamp( Dvar_GetFloat( light_power ), 5.0f, 30.0f ) );

    LOG_OK( "Scene '%s' loaded", scenePath );
    return true;
}

Scene& Com_GetScene()
{
    return g_scene;
}

void Com_UpdateWorld()
{
    Scene& scene = Com_GetScene();

    // update camera
    const ivec2 extent = g_wndMgr->FrameSize();
    const float aspect = (float)extent.x / extent.y;
    ASSERT( aspect > 0.0f );

    Camera& camera = scene.camera;
    ControlCamera( camera );
    camera.SetAspect( aspect );
    camera.UpdatePV();
}

static void ControlCamera( Camera& camera )
{
    constexpr float VIEW_SPEED = 2.0f;
    float CAMERA_SPEED = 0.15f;

    if ( ImGui::IsKeyDown( ImGuiKey_LeftShift ) )
        CAMERA_SPEED *= 3.f;

    int x = ImGui::IsKeyDown( ImGuiKey_D ) - ImGui::IsKeyDown( ImGuiKey_A );
    int y = ImGui::IsKeyDown( ImGuiKey_E ) - ImGui::IsKeyDown( ImGuiKey_Q );
    int z = ImGui::IsKeyDown( ImGuiKey_W ) - ImGui::IsKeyDown( ImGuiKey_S );

    if ( x != 0 || z != 0 ) {
        vec3 w = camera.direction();
        vec3 u = glm::cross( w, vec3( 0, 1, 0 ) );
        vec3 translation = ( CAMERA_SPEED * z ) * w + ( CAMERA_SPEED * x ) * u;
        camera.position += translation;
    }

    camera.position.y += ( CAMERA_SPEED * y );

    int yaw = ImGui::IsKeyDown( ImGuiKey_RightArrow ) - ImGui::IsKeyDown( ImGuiKey_LeftArrow );
    int pitch = ImGui::IsKeyDown( ImGuiKey_UpArrow ) - ImGui::IsKeyDown( ImGuiKey_DownArrow );

    if ( yaw ) {
        camera.yaw += VIEW_SPEED * yaw;
    }

    if ( pitch ) {
        camera.pitch += VIEW_SPEED * pitch;
        camera.pitch = glm::clamp( camera.pitch, -80.0f, 80.0f );
    }
}