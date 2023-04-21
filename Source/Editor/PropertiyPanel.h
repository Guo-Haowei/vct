#pragma once

#include "Panel.h"

#include "Engine/Scene/Entity.h"

class PropertyPanel : public Panel
{
public:
    PropertyPanel() : Panel("Properties") {}

protected:
    virtual void RenderInternal(Scene& scene) override;
};
