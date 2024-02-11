#pragma once
#include "panel.h"

class RenderGraphEditor : public Panel {
public:
    RenderGraphEditor() : Panel("Render Graph") {}

protected:
    void RenderInternal(Scene& scene) override;
};
