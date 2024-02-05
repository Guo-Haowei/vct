#pragma once
#include "Engine/Framework/Application.h"
#include "Engine/Scene/Scene.h"
#include "panels/panel.h"

class EditorLayer : public Layer {
public:
    EditorLayer();

    virtual void Attach() override {}
    virtual void Update(float dt) override;
    virtual void Render() override;

private:
    void DockSpace();
    void AddPanel(std::shared_ptr<Panel> panel);

    std::vector<std::shared_ptr<Panel>> mPanels;
    ecs::Entity mSelected;
};