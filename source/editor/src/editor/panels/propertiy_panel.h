#pragma once
#include "panel.h"

namespace vct {

class PropertyPanel : public Panel {
public:
    PropertyPanel() : Panel("Properties") {}

protected:
    void update_internal(Scene& scene) override;
};

}  // namespace vct
