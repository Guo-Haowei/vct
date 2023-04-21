#pragma once

#include "Panel.h"

#include "Engine/Scene/Entity.h"

class PropertyPanel : public Panel
{
public:
    PropertyPanel() : Panel("Properties") {}

    void SetSelectedRef(ecs::Entity* pSelected) { mpSelected = pSelected; }

protected:
    virtual void RenderInternal(Scene& scene) override;

private:
    ecs::Entity* mpSelected = nullptr;
};
