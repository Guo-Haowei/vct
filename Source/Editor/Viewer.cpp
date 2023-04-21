#include "Viewer.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Framework/WindowManager.h"
#include "Engine/Math/Ray.h"

#include "imgui/imgui_internal.h"

// @TODO: refactor
#include "Engine/Core/camera.h"
extern uint32_t gFinalImage;

static void ControlCamera(Camera& camera);

void Viewer::Update(float)
{
    if (IsFocused())
    {
        ControlCamera(gCamera);
    }
}

void Viewer::RenderInternal(Scene& scene)
{
    constexpr float ratio = 1920.0f / 1080.0f;
    ImVec2 contentSize = ImGui::GetWindowSize();
    if (contentSize.y * ratio > contentSize.x)
    {
        contentSize.y = contentSize.x / ratio;
    }
    else
    {
        contentSize.x = contentSize.y * ratio;
    }

    if (IsFocused())
    {
        if (Input::IsButtonPressed(EMouseButton::LEFT))
        {
            ImVec2 pos = GImGui->NavWindow->ContentRegionRect.Min;
            auto [windowX, windowY] = gWindowManager->GetWindowPos();
            pos.x -= windowX;
            pos.y -= windowY;
            vec2 clicked = Input::GetCursor();
            clicked.x = (clicked.x - pos.x) / contentSize.x;
            clicked.y = (clicked.y - pos.y) / contentSize.y;

            if (clicked.x >= 0.0f && clicked.x <= 1.0f && clicked.y >= 0.0f && clicked.y <= 1.0f)
            {
                clicked *= 2.0f;
                clicked -= 1.0f;

                const Camera& camera = gCamera;
                const mat4& PV = camera.ProjView();
                const mat4 invPV = glm::inverse(PV);

                const vec3 rayStart = camera.position;
                const vec3 direction = glm::normalize(vec3(invPV * vec4(clicked.x, -clicked.y, 1.0f, 1.0f)));
                const vec3 rayEnd = rayStart + direction * camera.zFar;
                Ray ray(rayStart, rayEnd);

                const auto intersectionResult = scene.Intersects(ray);

                if (intersectionResult.entity.IsValid())
                {
                    SetSelected(intersectionResult.entity);
                }
            }
        }
        else if (Input::IsButtonPressed(EMouseButton::RIGHT))
        {
            SetSelected(ecs::Entity::INVALID);
        }
    }

    ImVec2 topLeft = GImGui->CurrentWindow->ContentRegionRect.Min;
    ImVec2 bottomRight(topLeft.x + contentSize.x, topLeft.y + contentSize.y);

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
