#include "Editor.h"

#include "Engine/Core/CommonDvars.h"
#include "Engine/Core/Check.h"
#include "Engine/Core/DynamicVariable.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Graphics/r_cbuffers.h"
#include "Engine/Math/Ray.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Framework/WindowManager.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

class EditorLayer : public Layer
{
public:
    EditorLayer() = default;

    virtual void Attach() override {}
    virtual void Render() override {}
    virtual void Update(float dt) override;

private:
    void DbgWindow();
    void DockSpace();

    ImVec2 pos;
    ImVec2 size;
};

static const char* DrawTextureToStr(int mode)
{
    const char* str = "scene";
    switch (mode)
    {
        case TEXTURE_VOXEL_ALBEDO:
            str = "voxel color";
            break;
        case TEXTURE_VOXEL_NORMAL:
            str = "voxel normal";
            break;
        case TEXTURE_GBUFFER_DEPTH:
            str = "depth";
            break;
        case TEXTURE_GBUFFER_ALBEDO:
            str = "albedo";
            break;
        case TEXTURE_GBUFFER_NORMAL:
            str = "normal";
            break;
        case TEXTURE_GBUFFER_METALLIC:
            str = "metallic";
            break;
        case TEXTURE_GBUFFER_ROUGHNESS:
            str = "roughness";
            break;
        case TEXTURE_GBUFFER_SHADOW:
            str = "shadow";
            break;
        case TEXTURE_SSAO:
            str = "ssao";
            break;
        default:
            break;
    }
    return str;
}

void EditorLayer::DbgWindow()
{
    if (ImGui::Begin("Debug"))
    {
        Scene& scene = Com_GetScene();
        const Camera& camera = scene.camera;
        bool dirty = false;

        const vec3& eye = camera.position;
        ImGui::Text("eye: %.2f, %.2f, %.2f", eye.x, eye.y, eye.z);
        ImGui::Separator();

        ImGui::Text("Voxel GI");
        ImGui::Checkbox("Enable GI", (bool*)(DVAR_GET_POINTER(r_enableVXGI)));
        ImGui::Checkbox("No Texture", (bool*)(DVAR_GET_POINTER(r_noTexture)));
        dirty |= ImGui::Checkbox("Force Voxel GI texture update", (bool*)(DVAR_GET_POINTER(r_forceVXGI)));
        ImGui::Separator();

        ImGui::Text("CSM");
        ImGui::Checkbox("Debug CSM", (bool*)(DVAR_GET_POINTER(r_debugCSM)));
        ImGui::Separator();

        ImGui::Text("SSAO");
        ImGui::Checkbox("Enable SSAO", (bool*)(DVAR_GET_POINTER(r_enableSsao)));
        ImGui::Text("SSAO Kernal Radius");
        ImGui::SliderFloat("Kernal Radius", (float*)(DVAR_GET_POINTER(r_ssaoKernelRadius)), 0.1f, 5.0f);
        ImGui::Separator();

        ImGui::Text("FXAA");
        ImGui::Checkbox("Enable FXAA", (bool*)(DVAR_GET_POINTER(r_enableFXAA)));
        ImGui::Separator();

        ImGui::Text("Display Texture");
        ImGui::SliderInt("Display Texture", (int*)(DVAR_GET_POINTER(r_debugTexture)), DrawTexture::TEXTURE_FINAL_IMAGE, DrawTexture::TEXTURE_MAX);
        ImGui::Text("%s", DrawTextureToStr(DVAR_GET_INT(r_debugTexture)));

        ImGui::Separator();
        ImGui::Text("Light");
        float* lightDir = (float*)DVAR_GET_POINTER(light_dir);
        dirty |= ImGui::SliderFloat("x", lightDir, -20.f, 20.f);
        dirty |= ImGui::SliderFloat("z", lightDir + 2, -20.f, 20.f);

        ImGui::Text("Floor");
        if (scene.selected)
        {
            void* mat = scene.selected->material->gpuResource;
            MaterialData* drawData = reinterpret_cast<MaterialData*>(mat);
            dirty |= ImGui::SliderFloat("metallic", &drawData->metallic, 0.0f, 1.0f);
            dirty |= ImGui::SliderFloat("roughness", &drawData->roughness, 0.0f, 1.0f);
        }

        scene.light.direction = glm::normalize(DVAR_GET_VEC3(light_dir));
        scene.dirty = dirty;
    }
    ImGui::End();
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
        DbgWindow();
    }

    ImGuiIO& io = ImGui::GetIO();
    Scene& scene = Com_GetScene();

    // select object
    if (!io.WantCaptureMouse && gWindowManager->IsMouseInScreen())
    {
        auto [mouseX, mouseY] = gWindowManager->GetMousePos();
        auto [frameW, frameH] = gWindowManager->GetFrameSize();
        if (Input::IsButtonPressed(EMouseButton::LEFT))
        {
#if 0
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
#endif
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
}

Editor::Editor() : Application(Application::InitInfo{ "Editor", 1920, 1080, false })
{
    AddLayer(std::make_shared<EditorLayer>());
}
