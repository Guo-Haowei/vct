#pragma once
#include "panel.h"

class DebugPanel : public Panel {
public:
    DebugPanel() : Panel("Debug") {}

protected:
    virtual void RenderInternal(Scene& scene) override;
};
