#pragma once
#include <string>

#include "core/systems/entity.h"
#include "imgui/imgui.h"
#include "scene/scene.h"

using namespace vct;

class Panel {
public:
    Panel(const std::string& name) : mName(name) {}

    virtual void Update(float) {}
    void Render(vct::Scene& scene);
    bool IsFocused() const;

    void SetSelectedRef(ecs::Entity* pSelected) { mpSelected = pSelected; }
    void SetSelected(ecs::Entity selected) { *mpSelected = selected; }
    ecs::Entity GetSelected() const { return *mpSelected; }

protected:
    virtual void RenderInternal(vct::Scene& scene) = 0;

    std::string mName;
    ecs::Entity* mpSelected = nullptr;
};
