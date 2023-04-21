#pragma once
#include "Panel.h"

#include "Engine/Scene/Entity.h"

class Scene;

class HierarchyPanel : public Panel
{
public:
    HierarchyPanel() : Panel("Hierarchy") {}

    void SetSelectedRef(ecs::Entity* pSelected) { mpSelected = pSelected; }

    ecs::Entity GetSelected() const { return *mpSelected; }
    void SetSelected(ecs::Entity selected) { *mpSelected = selected; }

protected:
    virtual void RenderInternal(Scene& scene) override;

private:
    ecs::Entity* mpSelected = nullptr;
};
