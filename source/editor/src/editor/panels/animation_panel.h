#pragma once
#include "panel.h"

namespace vct {

class AnimationPanel : public Panel {
public:
    AnimationPanel(EditorLayer& editor) : Panel("Animation", editor) {}

protected:
    void update_internal(Scene& scene) override;
};

}  // namespace vct
