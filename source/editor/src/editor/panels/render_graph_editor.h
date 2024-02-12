#pragma once
#include "panel.h"

namespace vct {

class RenderGraphEditor : public Panel {
public:
    RenderGraphEditor() : Panel("Render Graph") {}

protected:
    void update_internal(Scene& scene) override;
};

}  // namespace vct
