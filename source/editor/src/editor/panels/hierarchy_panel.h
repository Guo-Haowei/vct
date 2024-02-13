#pragma once
#include "panel.h"

namespace vct {

class HierarchyPanel : public Panel {
public:
    HierarchyPanel(EditorLayer& editor) : Panel("Scene", editor) {}

protected:
    void update_internal(Scene& scene) override;
};

}  // namespace vct
