#pragma once
#include "camera_controller.h"
#include "panel.h"

class Viewer : public Panel {
public:
    Viewer() : Panel("Viewer") {}

    void Update(float dt) override;

protected:
    void RenderInternal(Scene& scene) override;
};
