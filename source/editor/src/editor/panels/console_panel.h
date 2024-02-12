#pragma once
#include "panel.h"

namespace vct {

class ConsolePanel : public Panel {
public:
    ConsolePanel() : Panel("Console") {}

protected:
    void update_internal(Scene& scene) override;

private:
    bool mAutoScroll = true;
    bool mScrollToBottom = false;
};

}  // namespace vct
