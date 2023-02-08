#include "EditorLogic.hpp"

#include "Base/Asserts.h"
#include "Base/Logger.h"

// @TODO: remove
#include "Core/GlfwApplication.hpp"
#include "Manager/SceneManager.hpp"

#include "imgui/imgui.h"

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

void EditorLogic::Tick()
{
    auto* app = dynamic_cast<BaseApplication*>( GetAppPointer() );
    Scene* scene = app->GetSceneManager()->GetScene();

    // update camera
    int w, h;
    m_pApp->GetFramebufferSize( w, h );
    const float aspect = (float)w / h;
    ASSERT( aspect > 0.0f );

    Camera& camera = scene->camera;
    ControlCamera( camera );
    camera.SetAspect( aspect );
    camera.UpdatePV();
}
