#pragma once
#include "panel.h"

namespace vct {

class DebugPanel : public Panel {
public:
    DebugPanel(EditorLayer& editor) : Panel("Debug", editor) {}

protected:
    void update_internal(Scene& scene) override;
};

}  // namespace vct
