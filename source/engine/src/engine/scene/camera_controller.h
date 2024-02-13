#pragma once
#include "core/math/degree.h"
#include "scene/scene_components.h"

namespace vct {

class CameraController {
public:
    static constexpr float MAX_SCROLL_SPEED = 100.0f;

    void set_camera(CameraComponent* camera);

    void move(float dt);

private:
    vec3 calculate_eye(const vec3& center);

    // @TODO: serialize camera controller
    Degree m_angle_x = Degree(90);
    Degree m_angle_xz = Degree(30);

    float m_distance = 10;

    vec3 m_direction;

    float m_scroll_speed = 0.0f;
    CameraComponent* m_camera = nullptr;
};

}  // namespace vct
