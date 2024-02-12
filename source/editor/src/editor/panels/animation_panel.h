#pragma once
#include "panel.h"

namespace vct {

class AnimationPanel : public Panel {
public:
    AnimationPanel() : Panel("Animation") {}

protected:
    void update_internal(Scene& scene) override;
};

}  // namespace vct
