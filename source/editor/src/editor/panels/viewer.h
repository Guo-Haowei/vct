#pragma once
#include "camera_controller.h"
#include "panel.h"

namespace vct {

class Viewer : public Panel {
public:
    Viewer();

protected:
    void update_internal(Scene& scene) override;

private:
    enum State : uint8_t {
        GIZMO_TRANSLATE,
        GIZMO_ROTATE,
        GIZMO_SCALE,
    };

    void update_data();
    void update_camera(CameraComponent& camera, float dt);
    void update_picking(Scene& scene, const CameraComponent& camera);
    void update_gizmo(Scene& scene, CameraComponent& camera);

    vec2 m_canvas_min;
    vec2 m_canvas_size;
    CameraController m_camera_controller;
    bool m_focused;
    State m_state = GIZMO_TRANSLATE;
};

}  // namespace vct
