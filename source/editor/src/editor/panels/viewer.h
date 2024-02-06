#pragma once
#include "panel.h"

class Viewer : public Panel {
public:
    Viewer() : Panel("Viewer") {}

    virtual void Update(float dt) override;

protected:
    virtual void RenderInternal(Scene& scene) override;
};
