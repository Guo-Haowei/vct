#pragma once
#include "Panel.h"

class HierarchyPanel : public Panel {
public:
    HierarchyPanel() : Panel("Hierarchy") {}

protected:
    virtual void RenderInternal(Scene& scene) override;
};
