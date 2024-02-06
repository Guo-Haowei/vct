#pragma once
#include "panel.h"

class HierarchyPanel : public Panel {
public:
    HierarchyPanel() : Panel("Hierarchy") {}

protected:
    virtual void RenderInternal(Scene& scene) override;
};
