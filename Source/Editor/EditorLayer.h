#pragma once
#include "Panel.h"

#include "imgui/imgui.h"
#include "Engine/Framework/Application.h"

class EditorLayer : public Layer
{
public:
    EditorLayer();

    virtual void Attach() override {}
    virtual void Update(float dt) override;
    virtual void Render() override;

private:
    void DbgWindow();
    void DockSpace();

    std::vector<std::shared_ptr<Panel>> mPanels;

    // @TODO: refactor
    ImVec2 pos;
    ImVec2 size;
};