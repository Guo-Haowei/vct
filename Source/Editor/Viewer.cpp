#include "Viewer.h"

#include "Engine/Core/Input.h"
#include "Engine/Framework/SceneManager.h"

#include "imgui/imgui_internal.h"

// @TODO: refactor
extern uint32_t gFinalImage;

static void ControlCamera(Camera& camera);

void Viewer::Update(float dt)
{
    if (IsFocused())
    {
        Scene& scene = Com_GetScene();
        Camera& camera = scene.camera;
        ControlCamera(camera);
    }
}

void Viewer::RenderInternal()
{
    constexpr float ratio = 1920.0f / 1080.0f;
    ImVec2 size = ImGui::GetWindowSize();
    if (size.y * ratio > size.x)
    {
        size.y = size.x / ratio;
    }
    else
    {
        size.x = size.y * ratio;
    }

    ImGuiWindow* window = GImGui->CurrentWindow;
    ImVec2 topLeft = GImGui->CurrentWindow->Pos;
    ImVec2 bottomRight(topLeft.x + size.x, topLeft.y + size.y);

    ImGui::GetWindowDrawList()->AddImage(
        (ImTextureID)gFinalImage,
        topLeft, bottomRight,
        ImVec2(0, 1), ImVec2(1, 0));
}

static void ControlCamera(Camera& camera)
{
    constexpr float VIEW_SPEED = 2.0f;
    float CAMERA_SPEED = 0.15f;

    if (Input::IsKeyDown(EKeyCode::LEFT_SHIFT))
        CAMERA_SPEED *= 3.f;

    int x = Input::IsKeyDown(EKeyCode::D) - Input::IsKeyDown(EKeyCode::A);
    int z = Input::IsKeyDown(EKeyCode::W) - Input::IsKeyDown(EKeyCode::S);
    int y = Input::IsKeyDown(EKeyCode::E) - Input::IsKeyDown(EKeyCode::Q);

    if (x != 0 || z != 0)
    {
        vec3 w = camera.direction();
        vec3 u = glm::cross(w, vec3(0, 1, 0));
        vec3 translation = (CAMERA_SPEED * z) * w + (CAMERA_SPEED * x) * u;
        camera.position += translation;
    }

    camera.position.y += (CAMERA_SPEED * y);

    int yaw = Input::IsKeyDown(EKeyCode::RIGHT) - Input::IsKeyDown(EKeyCode::LEFT);
    int pitch = Input::IsKeyDown(EKeyCode::UP) - Input::IsKeyDown(EKeyCode::DOWN);

    if (yaw)
    {
        camera.yaw += VIEW_SPEED * yaw;
    }

    if (pitch)
    {
        camera.pitch += VIEW_SPEED * pitch;
        camera.pitch = glm::clamp(camera.pitch, -80.0f, 80.0f);
    }
}
