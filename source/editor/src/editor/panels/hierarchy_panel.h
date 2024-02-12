#pragma once
#include "panel.h"

namespace vct {

class HierarchyPanel : public Panel {
public:
    HierarchyPanel() : Panel("Hierarchy") {}

protected:
    void update_internal(Scene& scene) override;
};

}  // namespace vct
