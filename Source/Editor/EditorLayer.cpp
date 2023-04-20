#include "EditorLayer.h"

#include "ConsolePanel.h"
#include "DebugPanel.h"
#include "Viewer.h"

#include "Engine/Core/CommonDvars.h"
#include "Engine/Core/Check.h"
#include "Engine/Core/DynamicVariable.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Graphics/r_cbuffers.h"
#include "Engine/Math/Ray.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Framework/WindowManager.h"

#include "imgui/imgui_internal.h"

EditorLayer::EditorLayer() : Layer("EditorLayer")
{
    mPanels.emplace_back(std::make_shared<ConsolePanel>());
    mPanels.emplace_back(std::make_shared<DebugPanel>());
    mPanels.emplace_back(std::make_shared<Viewer>());
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
    ImGuiIO& io = ImGui::GetIO();
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    const auto* node = ImGui::DockBuilderGetCentralNode(dockspace_id);
    check(node);

    pos = node->Pos;
    size = node->Size;

    ImGui::End();
    return;
#if 0
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

#if 0
    ImGuiIO& io = ImGui::GetIO();
    Scene& scene = Com_GetScene();

    // select object
    if (!io.WantCaptureMouse && gWindowManager->IsMouseInScreen())
    {
        auto [mouseX, mouseY] = gWindowManager->GetMousePos();
        auto [frameW, frameH] = gWindowManager->GetFrameSize();
        if (Input::IsButtonPressed(EMouseButton::LEFT))
        {
            const Camera& camera = scene.camera;

            const mat4& PV = camera.ProjView();
            const mat4 invPV = glm::inverse(PV);
            vec2 pos(mouseX / frameW, 1.0f - mouseY / frameH);
            pos -= 0.5f;
            pos *= 2.0f;

            vec3 rayStart = camera.position;
            vec3 direction = glm::normalize(vec3(invPV * vec4(pos.x, pos.y, 1.0f, 1.0f)));
            vec3 rayEnd = rayStart + direction * camera.zFar;
            Ray ray(rayStart, rayEnd);

            // @TODO: fix selection
            for (const auto& node : scene.geometryNodes)
            {
                for (const auto& geom : node.geometries)
                {
                    if (!geom.visible)
                    {
                        continue;
                    }
                    const AABB& box = geom.boundingBox;
                    const auto& mesh = geom.mesh;
                    if (ray.Intersects(box))
                    {
                        for (uint32_t idx = 0; idx < mesh->indices.size();)
                        {
                            const vec3& a = mesh->positions[mesh->indices[idx++]];
                            const vec3& b = mesh->positions[mesh->indices[idx++]];
                            const vec3& c = mesh->positions[mesh->indices[idx++]];
                            if (ray.Intersects(a, b, c))
                            {
                                scene.selected = &geom;
                            }
                        }
                    }
                }
            }
        }
        else if (Input::IsButtonPressed(EMouseButton::RIGHT))
        {
            scene.selected = nullptr;
        }
    }

    if (scene.selected && Input::IsKeyPressed(EKeyCode::DELETE))
    {
        if (scene.selected->visible)
        {
            LOG_WARN("material {} deleted", scene.selected->mesh->name);

            scene.selected->visible = false;
            scene.dirty = true;
            scene.selected = nullptr;
        }
    }
#endif

    for (auto& it : mPanels)
    {
        it->Update(dt);
    }
}

void EditorLayer::Render()
{
    for (auto& it : mPanels)
    {
        it->Render();
    }
}
