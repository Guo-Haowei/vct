#pragma once
#include "panel.h"

class AnimationPanel : public Panel {
public:
    AnimationPanel() : Panel("Animation") {}

protected:
    virtual void RenderInternal(Scene& scene) override;
};
