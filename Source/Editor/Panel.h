#pragma once
#include <string>

#include "imgui/imgui.h"

class Panel
{
public:
    Panel(const std::string& name) : mName(name) {}

    virtual void Update(float) {}
    void Render();
    bool IsFocused() const;

protected:
    virtual void RenderInternal() = 0;

    std::string mName;
};
