#pragma once
#include "core/framework/application.h"
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
    void dock_space();
    void add_panel(std::shared_ptr<Panel> panel);

    std::vector<std::shared_ptr<Panel>> m_panels;
    ecs::Entity m_selected;
};

}  // namespace vct