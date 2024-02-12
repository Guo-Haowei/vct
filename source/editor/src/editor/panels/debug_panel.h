#pragma once
#include "panel.h"

namespace vct {

class DebugPanel : public Panel {
public:
    DebugPanel() : Panel("Debug") {}

protected:
    void update_internal(Scene& scene) override;
};

}  // namespace vct
