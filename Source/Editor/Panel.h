#pragma once
#include <string>

#include "Engine/Scene/Entity.h"
#include "imgui/imgui.h"

class Scene;

class Panel
{
public:
    Panel(const std::string& name) : mName(name) {}

    virtual void Update(float) {}
    void Render(Scene& scene);
    bool IsFocused() const;

    void SetSelectedRef(ecs::Entity* pSelected) { mpSelected = pSelected; }
    void SetSelected(ecs::Entity selected) { *mpSelected = selected; }
    ecs::Entity GetSelected() const { return *mpSelected; }

protected:
    virtual void RenderInternal(Scene& scene) = 0;

    std::string mName;
    ecs::Entity* mpSelected = nullptr;
};
