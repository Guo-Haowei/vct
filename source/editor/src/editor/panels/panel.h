#pragma once
#include "imgui/imgui.h"
#include "scene/scene.h"

namespace vct {

class Panel {
public:
    Panel(const std::string& name) : m_name(name) {}

    void update(vct::Scene&);

    void set_selected_ref(ecs::Entity* selected) { m_selected = selected; }
    void set_selected(ecs::Entity selected) { *m_selected = selected; }
    ecs::Entity get_selected() const { return *m_selected; }

protected:
    virtual void update_internal(vct::Scene&) {}

    std::string m_name;
    ecs::Entity* m_selected = nullptr;
};

}  // namespace vct
