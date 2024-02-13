#pragma once
#include "panel.h"

namespace vct {

class ConsolePanel : public Panel {
public:
    ConsolePanel(EditorLayer& editor) : Panel("Console", editor) {}

protected:
    void update_internal(Scene& scene) override;

private:
    bool mAutoScroll = true;
    bool mScrollToBottom = false;
};

}  // namespace vct
