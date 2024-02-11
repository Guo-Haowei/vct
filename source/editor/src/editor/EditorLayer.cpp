#include "EditorLayer.h"

#include "imgui/imgui_internal.h"
#include "servers/rendering/r_cbuffers.h"
/////////////////////
#include "core/dynamic_variable/common_dvars.h"
#include "core/input/input.h"
#include "panels/animation_panel.h"
#include "panels/console_panel.h"
#include "panels/debug_panel.h"
#include "panels/hierarchy_panel.h"
#include "panels/menu_bar.h"
#include "panels/propertiy_panel.h"
#include "panels/render_graph_editor.h"
#include "panels/viewer.h"
#include "scene/scene_manager.h"
#include "servers/display_server.h"

EditorLayer::EditorLayer() : Layer("EditorLayer") {
    AddPanel(std::make_shared<RenderGraphEditor>());
    AddPanel(std::make_shared<AnimationPanel>());
    AddPanel(std::make_shared<ConsolePanel>());
    AddPanel(std::make_shared<DebugPanel>());
    AddPanel(std::make_shared<HierarchyPanel>());
    AddPanel(std::make_shared<PropertyPanel>());
    AddPanel(std::make_shared<Viewer>());

    s_controller.set_camera(SceneManager::get_scene().get_main_camera());
}

void EditorLayer::AddPanel(std::shared_ptr<Panel> panel) {
    mPanels.emplace_back(panel);
    panel->SetSelectedRef(&mSelected);
}

void EditorLayer::DockSpace() {
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

void EditorLayer::Update(float dt) {
    DockSpace();
    for (auto& it : mPanels) {
        it->Update(dt);
    }
}

void EditorLayer::Render() {
    Scene& scene = SceneManager::get_scene();
    for (auto& it : mPanels) {
        it->Render(scene);
    }

    scene.m_selected = mSelected;
}
