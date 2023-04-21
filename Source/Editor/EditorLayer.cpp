#include "EditorLayer.h"

#include "ConsolePanel.h"
#include "DebugPanel.h"
#include "HierarchyPanel.h"
#include "PropertiyPanel.h"
#include "Viewer.h"

#include "Engine/Core/CommonDvars.h"
#include "Engine/Core/Check.h"
#include "Engine/Core/DynamicVariable.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Graphics/r_cbuffers.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Framework/WindowManager.h"

#include "imgui/imgui_internal.h"

EditorLayer::EditorLayer() : Layer("EditorLayer")
{
    AddPanel(std::make_shared<ConsolePanel>());
    AddPanel(std::make_shared<DebugPanel>());
    AddPanel(std::make_shared<HierarchyPanel>());
    AddPanel(std::make_shared<PropertyPanel>());
    AddPanel(std::make_shared<Viewer>());
}

void EditorLayer::AddPanel(std::shared_ptr<Panel> panel)
{
    mPanels.emplace_back(panel);
    panel->SetSelectedRef(&mSelected);
}

void EditorLayer::DockSpace()
{
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
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    if (!opt_padding)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    }
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    if (!opt_padding)
    {
        ImGui::PopStyleVar();
    }

    ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    ImGui::End();
    return;
#if 0
    // ImGuiIO& io = ImGui::GetIO();
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();

            if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
            }
            if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            }
            if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
            }
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            }
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            }
            ImGui::Separator();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
#endif
}

void EditorLayer::Update(float dt)
{
    static bool hideUI = false;
    if (Input::IsKeyPressed(EKeyCode::ESCAPE))
    {
        hideUI = !hideUI;
    }

    if (!hideUI)
    {
        DockSpace();
    }
    for (auto& it : mPanels)
    {
        it->Update(dt);
    }
}

void EditorLayer::Render()
{
    Scene& scene = Com_GetScene();
    for (auto& it : mPanels)
    {
        it->Render(scene);
    }

    scene.mSelected = mSelected;
}
