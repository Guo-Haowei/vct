#include "Viewer.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Framework/WindowManager.h"
#include "Engine/Math/Ray.h"

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

static void ray_cast(const vec2& point)
{
    ImGuiIO& io = ImGui::GetIO();
    Scene& scene = Com_GetScene();
    const Camera& camera = scene.camera;
    const mat4& PV = camera.ProjView();
    const mat4 invPV = glm::inverse(PV);

    vec3 rayStart = camera.position;
    vec3 direction = glm::normalize(vec3(invPV * vec4(point.x, point.y, 1.0f, 1.0f)));
    vec3 rayEnd = rayStart + direction * camera.zFar;
    Ray ray(rayStart, rayEnd);

    // WIP: Ray casting
    for (const auto& node : scene.geometryNodes)
    {
        for (const auto& geom : node.geometries)
        {
            if (!geom.visible)
            {
                continue;
            }
            const AABB& box = geom.boundingBox;
            const auto& mesh = geom.mesh;
            if (ray.Intersects(box))
            {
                for (uint32_t idx = 0; idx < mesh->indices.size();)
                {
                    const vec3& a = mesh->positions[mesh->indices[idx++]];
                    const vec3& b = mesh->positions[mesh->indices[idx++]];
                    const vec3& c = mesh->positions[mesh->indices[idx++]];
                    if (ray.Intersects(a, b, c))
                    {
                        scene.selected = &geom;
                        LOG_INFO("SELECTED");
                    }
                }
            }
        }
    }
}

#if 0

    // select object
    {
        {
        }
        else if (Input::IsButtonPressed(EMouseButton::RIGHT))
        {
            scene.selected = nullptr;
        }
    }

    if (scene.selected && Input::IsKeyPressed(EKeyCode::DELETE))
    {
        if (scene.selected->visible)
        {
            LOG_WARN("material {} deleted", scene.selected->mesh->name);

            scene.selected->visible = false;
            scene.dirty = true;
            scene.selected = nullptr;
        }
    }
#endif

void Viewer::RenderInternal()
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
            ray_cast(clicked);
            LOG_INFO("Left button clicked {}, {}", clicked.x, clicked.y);
        }
    }

    ImGuiWindow* window = GImGui->CurrentWindow;
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
