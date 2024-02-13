#include "camera_controller.h"

#include "core/input/input.h"

namespace vct {

void CameraController::set_camera(CameraComponent* camera) {
    if (m_camera) {
        return;
    }

    vec3 eye = calculate_eye(camera->get_eye());
    camera->set_eye(eye);

    m_direction = glm::normalize(eye - camera->get_center());
    m_camera = camera;
}

void CameraController::move(float dt) {
    // rotate
    if (input::is_button_down(MOUSE_BUTTON_MIDDLE)) {
        const float rotateSpeed = 20.0f * dt;
        vec2 p = input::mouse_move();
        bool dirty = false;
        if (p.x != 0.0f) {
            m_angle_x -= Degree(rotateSpeed * p.x);
            dirty = true;
        }
        if (p.y != 0.0f) {
            m_angle_xz += Degree(rotateSpeed * p.y);
            m_angle_xz.clamp(-80.0f, 80.0f);
            dirty = true;
        }

        if (dirty) {
            m_camera->set_eye(calculate_eye(m_camera->get_center()));
        }
        return;
    }

    // pan
    if (input::is_button_down(MOUSE_BUTTON_RIGHT)) {
        vec2 p = input::mouse_move();
        if (glm::abs(p.x) >= 1.0f || glm::abs(p.y) >= 1.0f) {
            const float panSpeed = 10.0f * dt;

            mat4 model = glm::inverse(m_camera->get_view_matrix());
            vec3 offset = glm::normalize(vec3(p.x, p.y, 0.0f));
            vec3 new_center = m_camera->get_center();
            new_center -= panSpeed * vec3(model * vec4(offset, 0.0f));
            m_camera->set_center(new_center);
            m_camera->set_eye(calculate_eye(m_camera->get_center()));
            return;
        }
    }

    // scroll
    // @TODO: push point forward
    const float accel = 100.0f;
    float scrolling = input::get_wheel().y;
    if (scrolling != 0.0f) {
        m_scroll_speed += dt * accel;
        m_scroll_speed = glm::min(m_scroll_speed, MAX_SCROLL_SPEED);
    } else {
        m_scroll_speed -= 10.0f;
        m_scroll_speed = glm::max(m_scroll_speed, 0.0f);
    }

    if (m_scroll_speed != 0.0f) {
        m_distance -= m_scroll_speed * scrolling;
        m_distance = glm::clamp(m_distance, 0.3f, 10000.0f);
        m_camera->set_eye(calculate_eye(m_camera->get_center()));
    }
}

vec3 CameraController::calculate_eye(const vec3& center) {
    const float y = m_distance * m_angle_xz.sin();
    const float xz = m_distance * m_angle_xz.cos();
    const float x = xz * m_angle_x.sin();
    const float z = xz * m_angle_x.cos();
    return center + vec3(x, y, z);
}

}  // namespace vct
