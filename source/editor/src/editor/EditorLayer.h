#pragma once
#include "Engine/Framework/Application.h"
#include "panels/panel.h"
#include "scene/scene.h"

namespace vct {

class EditorLayer : public Layer {
public:
    EditorLayer();

    void Attach() override {}
    void Update(float dt) override;
    void Render() override;

private:
    void DockSpace();
    void AddPanel(std::shared_ptr<Panel> panel);

    std::vector<std::shared_ptr<Panel>> mPanels;
    ecs::Entity mSelected;
};

}  // namespace vct