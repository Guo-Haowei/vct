#include "viewer.h"

#include "ImGuizmo.h"
#include "imgui/imgui_internal.h"

// @TODO: refactor
#include "core/dynamic_variable/common_dvars.h"
#include "core/input/input.h"
#include "core/math/ray.h"
#include "scene/scene_manager.h"
#include "servers/display_server.h"
#include "servers/rendering_server.h"

void Viewer::Update(float dt) {
    if (IsFocused()) {
        Scene& scene = SceneManager::get_scene();
        s_controller.move_camera(scene.get_main_camera(), dt);
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
        if (input::is_button_pressed(MOUSE_BUTTON_LEFT)) {

            auto [windowX, windowY] = DisplayServer::singleton().get_window_pos();
            vec2 clicked = input::get_cursor();
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

                if (intersectionResult.entity.is_valid()) {
                    SetSelected(intersectionResult.entity);
                }
            }
        } else if (input::is_button_pressed(MOUSE_BUTTON_RIGHT)) {
            SetSelected(ecs::Entity::kInvalid);
        }
    }

    ImVec2 top_left = GImGui->CurrentWindow->ContentRegionRect.Min;
    ImVec2 bottom_right(top_left.x + contentSize.x, top_left.y + contentSize.y);
    ImVec2 top_right = ImVec2(bottom_right.x, top_left.y);

    uint64_t final_image = RenderingServer::singleton().get_final_image();
    ImGui::GetWindowDrawList()->AddImage((ImTextureID)final_image, top_left, bottom_right, ImVec2(0, 1), ImVec2(1, 0));

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

    // @TODO: fix, only rotate now
    auto op = ImGuizmo::ROTATE;
    // draw gizmo
    if (mpSelected->is_valid()) {
        TransformComponent* transform = scene.get_component<TransformComponent>(*mpSelected);

        if (transform) {
            auto objComponent = scene.get_component<ObjectComponent>(*mpSelected);
            if (objComponent) {
                auto meshComponent = scene.get_component<MeshComponent>(objComponent->meshID);
                DEV_ASSERT(meshComponent);
                AABB aabb = meshComponent->local_bound;
                aabb.apply_matrix(transform->get_world_matrix());

                const mat4 M = glm::translate(mat4(1), aabb.center()) * glm::scale(mat4(1), aabb.size());
                ImGuizmo::draw_box_wireframe(projection_view_matrix, M);
            }

            mat4 local = transform->get_local_matrix();

            ImGuizmo::Manipulate(glm::value_ptr(view_matrix), glm::value_ptr(projection_matrix), op, ImGuizmo::WORLD, glm::value_ptr(local), nullptr, nullptr, nullptr, nullptr);

            transform->set_local_transform(local);
        }
    }

    // view cube
    const float size = 120.f;
    ImGuizmo::ViewManipulate((float*)&view_matrix[0].x, 10.0f, ImVec2(top_right.x - size, top_right.y), ImVec2(size, size), IM_COL32(64, 64, 64, 96));
}