#pragma once
#include "panel.h"

namespace vct {

class PropertyPanel : public Panel {
public:
    PropertyPanel(EditorLayer& editor) : Panel("Properties", editor) {}

protected:
    void update_internal(Scene& scene) override;
};

}  // namespace vct
