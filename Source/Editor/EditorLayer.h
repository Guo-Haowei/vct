#pragma once
#include "ConsolePanel.h"

#include "imgui/imgui.h"
#include "Engine/Framework/Application.h"

class EditorLayer : public Layer
{
public:
    EditorLayer() : Layer("Editor Layer") {}

    virtual void Attach() override {}
    virtual void Update(float dt) override;
    virtual void Render() override;

private:
    void DbgWindow();
    void DockSpace();

    ConsolePanel mConsolePanel;

    // @TODO: refactor
    ImVec2 pos;
    ImVec2 size;
};