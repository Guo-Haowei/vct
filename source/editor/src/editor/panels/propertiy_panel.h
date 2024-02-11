#pragma once

#include "core/systems/entity.h"
#include "panel.h"

class PropertyPanel : public Panel {
public:
    PropertyPanel() : Panel("Properties") {}

protected:
    void RenderInternal(Scene& scene) override;
};
