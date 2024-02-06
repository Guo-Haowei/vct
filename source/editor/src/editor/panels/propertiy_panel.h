#pragma once

#include "Engine/Scene/Entity.h"
#include "panel.h"

class PropertyPanel : public Panel {
public:
    PropertyPanel() : Panel("Properties") {}

protected:
    virtual void RenderInternal(Scene& scene) override;
};