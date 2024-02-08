#include "viewer.h"

#include "Engine/Core/Input.h"
#include "ImGuizmo.h"
#include "imgui/imgui_internal.h"

// @TODO: refactor
#include "core/dynamic_variable/common_dvars.h"
#include "core/math/ray.h"
#include "scene/scene_manager.h"
#include "servers/display_server.h"

extern uint32_t g_final_image;

CameraController s_controller;

void Viewer::Update(float dt) {
    if (IsFocused()) {
        Scene& scene = SceneManager::get_scene();
        s_controller.move_camera(scene.get_main_camera(), 0.016f);
    }
}

void Viewer::RenderInternal(Scene& scene) {
    CameraComponent& camera = scene.get_main_camera();

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

    const mat4 view_matrix = camera.get_view_matrix();
    const mat4 projection_matrix = camera.get_projection_matrix();
    const mat4 projection_view_matrix = camera.get_projection_view_matrix();

    if (IsFocused()) {
        if (Input::IsButtonPressed(EMouseButton::MIDDLE)) {

            auto [windowX, windowY] = DisplayServer::singleton().get_window_pos();
            vec2 clicked = Input::GetCursor();
            clicked.x = (clicked.x + windowX - canvasMinToScreen.x) / contentSize.x;
            clicked.y = (clicked.y + windowY - canvasMinToScreen.y) / contentSize.y;

            if (clicked.x >= 0.0f && clicked.x <= 1.0f && clicked.y >= 0.0f && clicked.y <= 1.0f) {
                clicked *= 2.0f;
                clicked -= 1.0f;

                const mat4 invPV = glm::inverse(projection_view_matrix);

                const vec3 rayStart = camera.get_eye();
                const vec3 direction = glm::normalize(vec3(invPV * vec4(clicked.x, -clicked.y, 1.0f, 1.0f)));
                const vec3 rayEnd = rayStart + direction * camera.get_far();
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

    ImVec2 top_left = GImGui->CurrentWindow->ContentRegionRect.Min;
    ImVec2 bottom_right(top_left.x + contentSize.x, top_left.y + contentSize.y);
    ImVec2 top_right = ImVec2(bottom_right.x, top_left.y);

    ImGui::GetWindowDrawList()->AddImage((ImTextureID)g_final_image, top_left, bottom_right, ImVec2(0, 1), ImVec2(1, 0));

    // draw gizmo

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(top_left.x, top_left.y, contentSize.x, contentSize.y);

    // draw grid
    mat4 identity(1);

    if (DVAR_GET_BOOL(grid_visibility)) {
        ImGuizmo::draw_grid(projection_view_matrix, identity, 10.0f);
    }

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
                aabb.apply_matrix(transform->get_world_matrix());

                const mat4 M = glm::translate(mat4(1), aabb.center()) * glm::scale(mat4(1), aabb.size());
                ImGuizmo::draw_box_wireframe(projection_view_matrix, M);
            }

            mat4 local = transform->get_local_matrix();

            ImGuizmo::Manipulate(glm::value_ptr(view_matrix), glm::value_ptr(projection_matrix), op, ImGuizmo::WORLD, glm::value_ptr(local), nullptr, nullptr, nullptr, nullptr);

            transform->set_local_transform(local);
        }
    }

    // @TODO: manipulate view
    const float size = 120.f;
    ImGuizmo::ViewManipulate((float*)&view_matrix[0].x, 10.0f, ImVec2(top_right.x - size, top_right.y), ImVec2(size, size), IM_COL32(32, 32, 32, 96));
}

void CameraController::set_camera(CameraComponent& camera) {
    m_angle_xz = 30.0f;
    vec3 eye = calculate_eye(camera.get_eye());
    camera.set_eye(eye);

    m_direction = glm::normalize(eye - camera.get_center());
}

void CameraController::move_camera(CameraComponent& camera, float dt) {
    // rotate
    if (Input::IsButtonDown(EMouseButton::MIDDLE)) {
        const float rotateSpeed = 20.0f * dt;
        vec2 p = Input::MouseMove();
        bool dirty = false;
        if (p.x != 0.0f) {
            m_angle_x -= rotateSpeed * p.x;
            dirty = true;
        }
        if (p.y != 0.0f) {
            m_angle_xz += rotateSpeed * p.y;
            m_angle_xz = glm::clamp(m_angle_xz, -80.0f, 80.0f);
            dirty = true;
        }

        if (dirty) {
            camera.set_eye(calculate_eye(camera.get_center()));
        }
        return;
    }

    // pan
    if (Input::IsButtonDown(EMouseButton::RIGHT)) {
        vec2 p = Input::MouseMove();
        if (glm::abs(p.x) >= 1.0f || glm::abs(p.y) >= 1.0f) {
            const float panSpeed = 10.0f * dt;

            mat4 model = glm::inverse(camera.get_view_matrix());
            vec3 offset = glm::normalize(vec3(p.x, p.y, 0.0f));
            vec3 new_center = camera.get_center();
            new_center -= panSpeed * vec3(model * vec4(offset, 0.0f));
            camera.set_center(new_center);
            camera.set_eye(calculate_eye(camera.get_center()));
            return;
        }
    }

    // scroll
    const float accel = 100.0f;
    float scrolling = Input::Wheel().y;
    if (scrolling != 0.0f) {
        m_scrollSpeed += dt * accel;
        m_scrollSpeed = glm::min(m_scrollSpeed, MAX_SCROLL_SPEED);
    } else {
        m_scrollSpeed -= 10.0f;
        m_scrollSpeed = glm::max(m_scrollSpeed, 0.0f);
    }

    if (m_scrollSpeed != 0.0f) {
        m_distance -= m_scrollSpeed * scrolling;
        m_distance = glm::clamp(m_distance, 0.3f, 10000.0f);
        camera.set_eye(calculate_eye(camera.get_center()));
    }
}

vec3 CameraController::calculate_eye(const vec3& center) {
    const float rad_x = glm::radians(m_angle_x);
    const float rad_xz = glm::radians(m_angle_xz);
    const float y = m_distance * glm::sin(rad_xz);
    const float xz = m_distance * glm::cos(rad_xz);
    const float x = xz * glm::sin(rad_x);
    const float z = xz * glm::cos(rad_x);
    return center + vec3(x, y, z);
}
