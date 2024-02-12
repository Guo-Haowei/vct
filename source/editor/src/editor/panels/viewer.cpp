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

namespace vct {

Viewer::Viewer() : Panel("Viewer") {
    m_camera_controller.set_camera(SceneManager::get_scene().get_main_camera());
}

void Viewer::update_data() {
    auto [frame_width, frame_height] = DisplayServer::singleton().get_frame_size();
    const float ratio = (float)frame_width / frame_height;
    m_canvas_size.x = ImGui::GetWindowSize().x;
    m_canvas_size.y = ImGui::GetWindowSize().y;
    if (m_canvas_size.y * ratio > m_canvas_size.x) {
        m_canvas_size.y = m_canvas_size.x / ratio;
    } else {
        m_canvas_size.x = m_canvas_size.y * ratio;
    }

    ImGuiWindow* window = ImGui::FindWindowByName(m_name.c_str());
    DEV_ASSERT(window);
    m_canvas_min.x = window->ContentRegionRect.Min.x;
    m_canvas_min.y = window->ContentRegionRect.Min.y;

    m_focused = ImGui::IsWindowHovered();
}

void Viewer::update_camera(CameraComponent& camera, float dt) {
    if (m_focused) {
        m_camera_controller.move_camera(camera, dt);
    }
}

void Viewer::update_picking(Scene& scene, const CameraComponent& camera) {
    if (!m_focused) {
        return;
    }

    if (input::is_button_pressed(MOUSE_BUTTON_LEFT)) {
        auto [window_x, window_y] = DisplayServer::singleton().get_window_pos();
        vec2 clicked = input::get_cursor();
        clicked.x = (clicked.x + window_x - m_canvas_min.x) / m_canvas_size.x;
        clicked.y = (clicked.y + window_y - m_canvas_min.y) / m_canvas_size.y;

        if (clicked.x >= 0.0f && clicked.x <= 1.0f && clicked.y >= 0.0f && clicked.y <= 1.0f) {
            clicked *= 2.0f;
            clicked -= 1.0f;

            const mat4 inversed_projection_view = glm::inverse(camera.get_projection_view_matrix());

            const vec3 ray_start = camera.get_eye();
            const vec3 direction = glm::normalize(vec3(inversed_projection_view * vec4(clicked.x, -clicked.y, 1.0f, 1.0f)));
            const vec3 ray_end = ray_start + direction * camera.get_far();
            Ray ray(ray_start, ray_end);

            const auto intersection_result = scene.Intersects(ray);

            if (intersection_result.entity.is_valid()) {
                set_selected(intersection_result.entity);
            }
        }
    } else if (input::is_button_pressed(MOUSE_BUTTON_RIGHT)) {
        set_selected(ecs::Entity::INVALID);
    }
}

void Viewer::update_gizmo(Scene& scene, CameraComponent& camera) {
    const mat4 view_matrix = camera.get_view_matrix();
    const mat4 projection_matrix = camera.get_projection_matrix();
    const mat4 projection_view_matrix = camera.get_projection_view_matrix();

    // draw gizmo
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(m_canvas_min.x, m_canvas_min.y, m_canvas_size.x, m_canvas_size.y);

    // draw grid
    mat4 identity(1);

    if (DVAR_GET_BOOL(grid_visibility)) {
        ImGuizmo::draw_grid(projection_view_matrix, identity, 10.0f);
    }

    // draw gizmo
    if (m_selected->is_valid()) {
        if (input::is_key_pressed(KEY_T)) {
            m_state = GIZMO_TRANSLATE;
        } else if (input::is_key_pressed(KEY_R)) {
            m_state = GIZMO_ROTATE;
        } else if (input::is_key_pressed(KEY_S)) {
            m_state = GIZMO_SCALE;
        }

        ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
        switch (m_state) {
            case GIZMO_TRANSLATE:
                operation = ImGuizmo::TRANSLATE;
                break;
            case GIZMO_ROTATE:
                operation = ImGuizmo::ROTATE;
                break;
            case GIZMO_SCALE:
                operation = ImGuizmo::SCALE;
                break;
            default:
                break;
        }

        TransformComponent* transform_component = scene.get_component<TransformComponent>(*m_selected);

        if (transform_component) {
            auto object_component = scene.get_component<ObjectComponent>(*m_selected);
            if (object_component) {
                auto mesh_component = scene.get_component<MeshComponent>(object_component->mesh_id);
                DEV_ASSERT(mesh_component);
                AABB aabb = mesh_component->local_bound;
                aabb.apply_matrix(transform_component->get_world_matrix());

                const mat4 model_matrix = glm::translate(mat4(1), aabb.center()) * glm::scale(mat4(1), aabb.size());
                ImGuizmo::draw_box_wireframe(projection_view_matrix, model_matrix);
            }

            mat4 local = transform_component->get_local_matrix();

            ImGuizmo::Manipulate(glm::value_ptr(view_matrix), glm::value_ptr(projection_matrix), operation, ImGuizmo::LOCAL, glm::value_ptr(local), nullptr, nullptr, nullptr, nullptr);

            transform_component->set_local_transform(local);
        }
    }

    // view cube
    const float size = 120.f;
    ImGuizmo::ViewManipulate((float*)&view_matrix[0].x, 10.0f, ImVec2(m_canvas_min.x, m_canvas_min.y), ImVec2(size, size), IM_COL32(64, 64, 64, 96));
}

void Viewer::update_internal(Scene& scene) {
    CameraComponent& camera = scene.get_main_camera();

    update_data();
    update_camera(camera, scene.m_delta_time);
    update_picking(scene, camera);

    // add image for drawing
    ImVec2 top_left(m_canvas_min.x, m_canvas_min.y);
    ImVec2 bottom_right(top_left.x + m_canvas_size.x, top_left.y + m_canvas_size.y);

    uint64_t final_image = RenderingServer::singleton().get_final_image();
    ImGui::GetWindowDrawList()->AddImage((ImTextureID)final_image, top_left, bottom_right, ImVec2(0, 1), ImVec2(1, 0));

    update_gizmo(scene, camera);
}

}  // namespace vct