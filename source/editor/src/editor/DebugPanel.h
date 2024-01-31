#pragma once
#include "Panel.h"

class DebugPanel : public Panel {
public:
    DebugPanel() : Panel("Debug") {}

protected:
    virtual void RenderInternal(Scene& scene) override;
};
