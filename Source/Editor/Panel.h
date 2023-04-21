#pragma once
#include <string>

#include "imgui/imgui.h"

class Scene;

class Panel
{
public:
    Panel(const std::string& name) : mName(name) {}

    virtual void Update(float) {}
    void Render(Scene& scene);
    bool IsFocused() const;

protected:
    virtual void RenderInternal(Scene& scene) = 0;

    std::string mName;
};
