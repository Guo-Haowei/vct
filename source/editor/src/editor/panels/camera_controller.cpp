#include "camera_controller.h"

#include "core/input/input.h"

vct::CameraController s_controller;

namespace vct {

void CameraController::set_camera(CameraComponent& camera) {
    m_angle_xz = 30.0f;
    vec3 eye = calculate_eye(camera.get_eye());
    camera.set_eye(eye);

    m_direction = glm::normalize(eye - camera.get_center());
}

void CameraController::move_camera(CameraComponent& camera, float dt) {
    // rotate
    if (input::is_button_down(MOUSE_BUTTON_MIDDLE)) {
        const float rotateSpeed = 20.0f * dt;
        vec2 p = input::mouse_move();
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
    if (input::is_button_down(MOUSE_BUTTON_RIGHT)) {
        vec2 p = input::mouse_move();
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
    // @TODO: push point forward
    const float accel = 100.0f;
    float scrolling = input::get_wheel().y;
    if (scrolling != 0.0f) {
        m_scroll_speed += dt * accel;
        m_scroll_speed = glm::min(m_scroll_speed, kMaxScrollSpeed);
    } else {
        m_scroll_speed -= 10.0f;
        m_scroll_speed = glm::max(m_scroll_speed, 0.0f);
    }

    if (m_scroll_speed != 0.0f) {
        m_distance -= m_scroll_speed * scrolling;
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

}  // namespace vct
