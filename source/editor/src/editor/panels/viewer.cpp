#include "viewer.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/camera.h"
#include "ImGuizmo.h"
#include "imgui/imgui_internal.h"

// @TODO: refactor
#include "core/math/ray.h"
#include "scene/scene_manager.h"
#include "servers/display_server.h"

extern uint32_t g_final_image;

static void camera_control(Camera& camera);

void Viewer::Update(float) {
    if (IsFocused()) {
        camera_control(gCamera);
    }
}

void Viewer::RenderInternal(Scene& scene) {
    ImGuiWindow* window = ImGui::FindWindowByName(mName.c_str());
    DEV_ASSERT(window);
    const ImRect& contentRegionRect = window->ContentRegionRect;
    vec2 canvasMinToScreen;
    canvasMinToScreen.x = contentRegionRect.Min.x;
    canvasMinToScreen.y = contentRegionRect.Min.y;

    constexpr float ratio = 1920.0f / 1080.0f;
    vec2 contentSize;
    contentSize.x = ImGui::GetWindowSize().x;
    contentSize.y = ImGui::GetWindowSize().y;
    if (contentSize.y * ratio > contentSize.x) {
        contentSize.y = contentSize.x / ratio;
    } else {
        contentSize.x = contentSize.y * ratio;
    }

    if (IsFocused()) {
        if (Input::IsButtonPressed(EMouseButton::MIDDLE)) {

            auto [windowX, windowY] = DisplayServer::singleton().get_window_pos();
            vec2 clicked = Input::GetCursor();
            clicked.x = (clicked.x + windowX - canvasMinToScreen.x) / contentSize.x;
            clicked.y = (clicked.y + windowY - canvasMinToScreen.y) / contentSize.y;

            if (clicked.x >= 0.0f && clicked.x <= 1.0f && clicked.y >= 0.0f && clicked.y <= 1.0f) {
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

                if (intersectionResult.entity.IsValid()) {
                    SetSelected(intersectionResult.entity);
                }
            }
        } else if (Input::IsButtonPressed(EMouseButton::RIGHT)) {
            SetSelected(ecs::Entity::INVALID);
        }
    }

    ImVec2 topLeft = GImGui->CurrentWindow->ContentRegionRect.Min;
    ImVec2 bottomRight(topLeft.x + contentSize.x, topLeft.y + contentSize.y);

    ImGui::GetWindowDrawList()->AddImage((ImTextureID)g_final_image, topLeft, bottomRight, ImVec2(0, 1), ImVec2(1, 0));

    // draw gizmo

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(topLeft.x, topLeft.y, contentSize.x, contentSize.y);

    const mat4 view_matrix = gCamera.View();
    const mat4 projection_matrix = gCamera.Proj();
    const mat4 projection_view_matrix = projection_matrix * view_matrix;

    // draw grid
    mat4 identity(1);
    ImGuizmo::draw_grid(projection_view_matrix, identity, 10.0f);

    // @TODO: view cube
    // ImGuizmo::DrawCubes(&view_matrix[0].x, &projection_matrix[0].x, &identity[0].x, 1);

    // @TODO: fix
    auto op = ImGuizmo::ROTATE;
    // draw gizmo
    if (mpSelected->IsValid()) {
        TransformComponent* transform = scene.get_component<TransformComponent>(*mpSelected);

        if (transform) {
            auto objComponent = scene.get_component<ObjectComponent>(*mpSelected);
            if (objComponent) {
                auto meshComponent = scene.get_component<MeshComponent>(objComponent->meshID);
                DEV_ASSERT(meshComponent);
                AABB aabb = meshComponent->mLocalBound;
                aabb.apply_matrix(transform->GetWorldMatrix());

                const mat4 M = glm::translate(mat4(1), aabb.center()) * glm::scale(mat4(1), aabb.size());
                ImGuizmo::draw_box_wireframe(projection_view_matrix, M);
            }

            mat4 local = transform->GetLocalMatrix();

            ImGuizmo::Manipulate(glm::value_ptr(view_matrix), glm::value_ptr(projection_matrix), op, ImGuizmo::WORLD, glm::value_ptr(local), nullptr, nullptr, nullptr, nullptr);

            transform->SetLocalTransform(local);
        }
    }
}

// @TODO: refactor
static void camera_control(Camera& camera) {
    constexpr float VIEW_SPEED = 2.0f;
    float CAMERA_SPEED = 0.15f;

    if (Input::IsKeyDown(EKeyCode::LEFT_SHIFT)) CAMERA_SPEED *= 3.f;

    int x = Input::IsKeyDown(EKeyCode::D) - Input::IsKeyDown(EKeyCode::A);
    int z = Input::IsKeyDown(EKeyCode::W) - Input::IsKeyDown(EKeyCode::S);
    int y = Input::IsKeyDown(EKeyCode::E) - Input::IsKeyDown(EKeyCode::Q);

    if (x != 0 || z != 0) {
        vec3 w = camera.direction();
        vec3 u = glm::cross(w, vec3(0, 1, 0));
        vec3 translation = (CAMERA_SPEED * z) * w + (CAMERA_SPEED * x) * u;
        camera.position += translation;
    }

    camera.position.y += (CAMERA_SPEED * y);

    int yaw = Input::IsKeyDown(EKeyCode::RIGHT) - Input::IsKeyDown(EKeyCode::LEFT);
    int pitch = Input::IsKeyDown(EKeyCode::UP) - Input::IsKeyDown(EKeyCode::DOWN);

    if (yaw) {
        camera.yaw += VIEW_SPEED * yaw;
    }

    if (pitch) {
        camera.pitch += VIEW_SPEED * pitch;
        camera.pitch = glm::clamp(camera.pitch, -80.0f, 80.0f);
    }
}
