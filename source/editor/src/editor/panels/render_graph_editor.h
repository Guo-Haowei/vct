#pragma once
#include "panel.h"

namespace vct {

class RenderGraphEditor : public Panel {
public:
    RenderGraphEditor(EditorLayer& editor) : Panel("Render Graph", editor) {}

protected:
    void update_internal(Scene& scene) override;
};

}  // namespace vct
