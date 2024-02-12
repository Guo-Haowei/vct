#pragma once
#include "core/math/degree.h"
#include "scene/scene_components.h"

namespace vct {

class CameraController {
public:
    static constexpr float kMaxScrollSpeed = 100.0f;

    void set_camera(CameraComponent& camera);
    void move_camera(CameraComponent& camera, float dt);

private:
    vec3 calculate_eye(const vec3& center);

    Degree m_angle_x = 90.0f;
    Degree m_angle_xz = 0.0f;

    float m_distance = 10.0f;

    vec3 m_direction;

    float m_scroll_speed = 0.0f;
};

}  // namespace vct
