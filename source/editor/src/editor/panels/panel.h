#pragma once
#include "imgui/imgui.h"
#include "scene/scene.h"

namespace vct {

class EditorLayer;

class Panel {
public:
    Panel(const std::string& name, EditorLayer& editor) : m_name(name), m_editor(editor) {}

    void update(vct::Scene&);

protected:
    virtual void update_internal(vct::Scene&) {}

    std::string m_name;
    EditorLayer& m_editor;
};

}  // namespace vct
