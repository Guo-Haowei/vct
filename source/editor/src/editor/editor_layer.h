#pragma once
#include "core/base/noncopyable.h"
#include "core/framework/application.h"
#include "panels/panel.h"
#include "scene/scene.h"

namespace vct {

class EditorLayer : public Layer, public NonCopyable {
public:
    enum State {
        STATE_PICKING,
        STATE_TRANSLATE,
        STATE_ROTATE,
        STATE_SCALE,
    };

    EditorLayer();

    void attach() override {}
    void update(float dt) override;
    void render() override;

    void select_entity(ecs::Entity selected);
    ecs::Entity get_selected_entity() const { return m_selected; }
    State get_state() const { return m_state; }
    void set_state(State state) { m_state = state; }

private:
    void dock_space();
    void add_panel(std::shared_ptr<Panel> panel);

    std::vector<std::shared_ptr<Panel>> m_panels;
    ecs::Entity m_selected;
    State m_state{ STATE_PICKING };
};

}  // namespace vct