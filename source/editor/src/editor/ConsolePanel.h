#pragma once
#include "Panel.h"

class ConsolePanel : public Panel {
public:
    ConsolePanel() : Panel("Console") {}

protected:
    virtual void RenderInternal(Scene& scene) override;

private:
    bool mAutoScroll = true;
    bool mScrollToBottom = false;
};
