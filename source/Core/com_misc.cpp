#include "com_misc.h"

#include <filesystem>

#include "FileManager.h"
#include "imgui/imgui.h"
#include "WindowManager.h"
#include "AssimpLoader.hpp"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Graphics/r_cbuffers.h"

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

bool Com_ImGuiInit()
{
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    [[maybe_unused]] ImGuiIO& io = ImGui::GetIO();

#if 0
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text] = ImVec4( 1.000f, 1.000f, 1.000f, 1.000f );
    colors[ImGuiCol_TextDisabled] = ImVec4( 0.500f, 0.500f, 0.500f, 1.000f );
    colors[ImGuiCol_WindowBg] = ImVec4( 0.180f, 0.180f, 0.180f, 1.000f );
    colors[ImGuiCol_ChildBg] = ImVec4( 0.280f, 0.280f, 0.280f, 0.000f );
    colors[ImGuiCol_PopupBg] = ImVec4( 0.313f, 0.313f, 0.313f, 1.000f );
    colors[ImGuiCol_Border] = ImVec4( 0.266f, 0.266f, 0.266f, 1.000f );
    colors[ImGuiCol_BorderShadow] = ImVec4( 0.000f, 0.000f, 0.000f, 0.000f );
    colors[ImGuiCol_FrameBg] = ImVec4( 0.160f, 0.160f, 0.160f, 1.000f );
    colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.200f, 0.200f, 0.200f, 1.000f );
    colors[ImGuiCol_FrameBgActive] = ImVec4( 0.280f, 0.280f, 0.280f, 1.000f );
    colors[ImGuiCol_TitleBg] = ImVec4( 0.148f, 0.148f, 0.148f, 1.000f );
    colors[ImGuiCol_TitleBgActive] = ImVec4( 0.148f, 0.148f, 0.148f, 1.000f );
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.148f, 0.148f, 0.148f, 1.000f );
    colors[ImGuiCol_MenuBarBg] = ImVec4( 0.195f, 0.195f, 0.195f, 1.000f );
    colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.160f, 0.160f, 0.160f, 1.000f );
    colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.277f, 0.277f, 0.277f, 1.000f );
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.300f, 0.300f, 0.300f, 1.000f );
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_CheckMark] = ImVec4( 1.000f, 1.000f, 1.000f, 1.000f );
    colors[ImGuiCol_SliderGrab] = ImVec4( 0.391f, 0.391f, 0.391f, 1.000f );
    colors[ImGuiCol_SliderGrabActive] = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_Button] = ImVec4( 1.000f, 1.000f, 1.000f, 0.000f );
    colors[ImGuiCol_ButtonHovered] = ImVec4( 1.000f, 1.000f, 1.000f, 0.156f );
    colors[ImGuiCol_ButtonActive] = ImVec4( 1.000f, 1.000f, 1.000f, 0.391f );
    colors[ImGuiCol_Header] = ImVec4( 0.313f, 0.313f, 0.313f, 1.000f );
    colors[ImGuiCol_HeaderHovered] = ImVec4( 0.469f, 0.469f, 0.469f, 1.000f );
    colors[ImGuiCol_HeaderActive] = ImVec4( 0.469f, 0.469f, 0.469f, 1.000f );
    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.391f, 0.391f, 0.391f, 1.000f );
    colors[ImGuiCol_SeparatorActive] = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_ResizeGrip] = ImVec4( 1.000f, 1.000f, 1.000f, 0.250f );
    colors[ImGuiCol_ResizeGripHovered] = ImVec4( 1.000f, 1.000f, 1.000f, 0.670f );
    colors[ImGuiCol_ResizeGripActive] = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_Tab] = ImVec4( 0.098f, 0.098f, 0.098f, 1.000f );
    colors[ImGuiCol_TabHovered] = ImVec4( 0.352f, 0.352f, 0.352f, 1.000f );
    colors[ImGuiCol_TabActive] = ImVec4( 0.195f, 0.195f, 0.195f, 1.000f );
    colors[ImGuiCol_TabUnfocused] = ImVec4( 0.098f, 0.098f, 0.098f, 1.000f );
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 0.195f, 0.195f, 0.195f, 1.000f );
    colors[ImGuiCol_PlotLines] = ImVec4( 0.469f, 0.469f, 0.469f, 1.000f );
    colors[ImGuiCol_PlotLinesHovered] = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_PlotHistogram] = ImVec4( 0.586f, 0.586f, 0.586f, 1.000f );
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_TextSelectedBg] = ImVec4( 1.000f, 1.000f, 1.000f, 0.156f );
    colors[ImGuiCol_DragDropTarget] = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_NavHighlight] = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 1.000f, 0.391f, 0.000f, 1.000f );
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4( 0.000f, 0.000f, 0.000f, 0.586f );
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4( 0.000f, 0.000f, 0.000f, 0.586f );

    style->ChildRounding = 4.0f;
    style->FrameBorderSize = 1.0f;
    style->FrameRounding = 2.0f;
    style->GrabMinSize = 7.0f;
    style->PopupRounding = 2.0f;
    style->ScrollbarRounding = 12.0f;
    style->ScrollbarSize = 13.0f;
    style->TabBorderSize = 1.0f;
    style->TabRounding = 0.0f;
    style->WindowRounding = 4.0f;
#endif

    return true;
}

void Com_ImGuiShutdown()
{
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

    //if ( ImGui::IsKeyDown( GLFW_KEY_LEFT_SHIFT ) )
    //    CAMERA_SPEED *= 3.f;

    //int x = ImGui::IsKeyDown( GLFW_KEY_D ) - ImGui::IsKeyDown( GLFW_KEY_A );
    //int y = ImGui::IsKeyDown( GLFW_KEY_E ) - ImGui::IsKeyDown( GLFW_KEY_Q );
    //int z = ImGui::IsKeyDown( GLFW_KEY_W ) - ImGui::IsKeyDown( GLFW_KEY_S );
    int x = 0;
    int y = 0;
    int z = 0;

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