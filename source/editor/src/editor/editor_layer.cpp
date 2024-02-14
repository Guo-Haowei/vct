#include "editor_layer.h"

#include "imgui/imgui_internal.h"
#include "rendering/r_cbuffers.h"
/////////////////////
#include "core/framework/scene_manager.h"
#include "core/input/input.h"
#include "panels/animation_panel.h"
#include "panels/console_panel.h"
#include "panels/debug_panel.h"
#include "panels/hierarchy_panel.h"
#include "panels/menu_bar.h"
#include "panels/propertiy_panel.h"
#include "panels/render_graph_editor.h"
#include "panels/viewer.h"
#include "servers/display_server.h"

namespace vct {

EditorLayer::EditorLayer() : Layer("EditorLayer") {
    add_panel(std::make_shared<RenderGraphEditor>(*this));
    add_panel(std::make_shared<AnimationPanel>(*this));
    add_panel(std::make_shared<ConsolePanel>(*this));
    add_panel(std::make_shared<DebugPanel>(*this));
    add_panel(std::make_shared<HierarchyPanel>(*this));
    add_panel(std::make_shared<PropertyPanel>(*this));
    add_panel(std::make_shared<Viewer>(*this));
}

void EditorLayer::add_panel(std::shared_ptr<Panel> panel) {
    m_panels.emplace_back(panel);
}

void EditorLayer::select_entity(ecs::Entity selected) {
    m_selected = selected;
    m_state = STATE_PICKING;
}

void EditorLayer::dock_space() {
    ImGui::GetMainViewport();

    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |=
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    if (!opt_padding) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    }
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    if (!opt_padding) {
        ImGui::PopStyleVar();
    }

    ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    menu_bar();

    ImGui::End();
    return;
}

void EditorLayer::update(float) {
    dock_space();
    Scene& scene = SceneManager::get_scene();
    for (auto& it : m_panels) {
        it->update(scene);
    }

    scene.m_selected = m_selected;
}

void EditorLayer::render() {
}

}  // namespace vct
