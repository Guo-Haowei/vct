#include "Panel.h"

#include "imgui/imgui_internal.h"

bool Panel::IsFocused() const
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* focusedWindow = g.NavWindow;
    return strcmp(mName.c_str(), focusedWindow->Name) == 0;
}

void Panel::Render()
{
    if (ImGui::Begin(mName.c_str()))
    {
        RenderInternal();
    }
    ImGui::End();
}
