#include "PropertiyPanel.h"

#include "Engine/Scene/Scene.h"
#include "imgui/imgui_internal.h"
#include "ImGuizmo/ImGuizmo.h"

static constexpr float DEFAULT_COLUMN_WIDTH = 100.0f;

static bool draw_vec3_control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = DEFAULT_COLUMN_WIDTH);
static bool draw_drag_float(const char* tag, float* p, float speed, float min, float max, float columnWidth = DEFAULT_COLUMN_WIDTH);

template<typename T, typename UIFunction>
static void DrawComponent(const std::string& name, T* component, UIFunction uiFunction)
{
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
    if (component)
    {
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::Separator();
        bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
        ImGui::PopStyleVar();
        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
        if (ImGui::Button("-", ImVec2{ lineHeight, lineHeight }))
        {
            ImGui::OpenPopup("ComponentSettings");
        }

        bool removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings"))
        {
            if (ImGui::MenuItem("Remove component"))
            {
                removeComponent = true;
            }

            ImGui::EndPopup();
        }

        if (open)
        {
            uiFunction(*component);
            ImGui::TreePop();
        }

        if (removeComponent)
        {
            LOG_ERROR("TODO: implement remove component");
        }
    }
}

static bool draw_vec3_control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
{
    bool dirty = false;

    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushFont(boldFont);

    if (ImGui::Button("X"))
    {
        values.x = resetValue;
        dirty = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    dirty |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y"))
    {
        values.y = resetValue;
        dirty = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    dirty |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z"))
    {
        values.z = resetValue;
        dirty = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    dirty |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
    return dirty;
}

static bool draw_drag_float(const char* tag, float* p, float speed, float min, float max, float columnWidth)
{
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(tag);
    ImGui::NextColumn();
    auto dragFloatTag = fmt::format("##{}", tag);
    bool dirty = ImGui::DragFloat(dragFloatTag.c_str(), p, speed, min, max);
    ImGui::Columns(1);
    return dirty;
}

void PropertyPanel::RenderInternal(Scene& scene)
{
    ecs::Entity id = *mpSelected;

    if (!id.IsValid())
    {
        return;
    }

    TagComponent* tagComponent = scene.GetComponent<TagComponent>(id);
    if (!tagComponent)
    {
        LOG_WARN("Entity {} does not have name", id.GetID());
        return;
    }

    TagComponent::TagString tag = tagComponent->GetTag();
    if (ImGui::InputText("##Tag", tag.data(), tag.capacity(), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        tagComponent->GetTagRef() = tag;
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        if (ImGui::MenuItem("Rigid Body"))
        {
            LOG_ERROR("TODO: implement add component");
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

#if 0
    LightComponent* lightComponent = scene.GetComponent<LightComponent>(id);
    DrawComponent("Light", lightComponent, [&](LightComponent& light) {
        ImGui::Text("Light Component");
        ImGui::DragFloat3("color:", &light.color.x);
        ImGui::DragFloat("energy:", &light.energy);
    });
#endif

    TransformComponent* transformComponent = scene.GetComponent<TransformComponent>(id);
    DrawComponent("Transform", transformComponent, [&](TransformComponent& transform) {
        mat4 transformMatrix = transform.GetLocalMatrix();
        vec3 translation;
        vec3 rotation;
        vec3 scale;
        ImGuizmo::DecomposeMatrixToComponents(
            glm::value_ptr(transformMatrix),
            glm::value_ptr(translation),
            glm::value_ptr(rotation),
            glm::value_ptr(scale));

        bool dirty = false;
        bool wantTranslation = false, wantRotation = false, wantScale = false;
        // if (lightComponent)
        // {
        //     switch (lightComponent->type)
        //     {
        //         case LIGHT_TYPE_POINT:
        //             wantTranslation = true;
        //             break;
        //         case LIGHT_TYPE_OMNI:
        //             wantRotation = true;
        //             break;
        //         default:
        //             break;
        //     }
        // }
        // else
        {
            wantTranslation = wantRotation = wantScale = true;
        }

        if (wantTranslation)
        {
            dirty |= draw_vec3_control("translation", translation);
        }
        if (wantRotation)
        {
            dirty |= draw_vec3_control("rotation", rotation);
        }
        if (wantScale)
        {
            dirty |= draw_vec3_control("scale", scale, 1.0f);
        }
        if (dirty)
        {
            ImGuizmo::RecomposeMatrixFromComponents(
                glm::value_ptr(translation),
                glm::value_ptr(rotation),
                glm::value_ptr(scale),
                glm::value_ptr(transformMatrix));
            transform.SetLocalTransform(transformMatrix);
        }
    });

    RigidBodyPhysicsComponent* rigidBodyComponent = scene.GetComponent<RigidBodyPhysicsComponent>(id);
    DrawComponent("RigidBody", rigidBodyComponent, [](RigidBodyPhysicsComponent& rigidbody) {
        switch (rigidbody.shape)
        {
            case RigidBodyPhysicsComponent::BOX: {
                const auto& half = rigidbody.param.box.halfExtent;
                ImGui::Text("shape: box");
                ImGui::Text("half size: %.2f, %.2f, %.2f", half.x, half.y, half.z);
                break;
            }
            default:
                break;
        }
    });

    CameraComponent* cameraComponent = scene.GetComponent<CameraComponent>(id);
    DrawComponent("Camera", cameraComponent, [](CameraComponent& camera) {
        const float width = 50.0f;
        float fovy = glm::degrees(camera.fovy);
        draw_drag_float("fovy", &fovy, 1.0f, 20.0f, 90.0f, width);
        draw_drag_float("near", &camera.zNear, 0.1f, 0.1f, 10.0f, width);
        draw_drag_float("far", &camera.zFar, 10.0f, 10.0f, 5000.0f, width);
        camera.fovy = glm::radians(fovy);
    });

    ObjectComponent* objectComponent = scene.GetComponent<ObjectComponent>(id);
    DrawComponent("Object", objectComponent, [&](ObjectComponent& object) {
        MeshComponent* mesh = scene.GetComponent<MeshComponent>(object.meshID);
        TagComponent* meshName = scene.GetComponent<TagComponent>(object.meshID);
        ImGui::Text("Mesh Component (%d)", object.meshID);
        if (mesh)
        {
            const char* meshNameStr = meshName ? meshName->GetTag().c_str() : "untitled";
            ImGui::Text("mesh %s (%zu submesh)", meshNameStr, mesh->mSubsets.size());
            ImGui::Text("%zu triangles", mesh->mIndices.size() / 3);
            ImGui::Text("v:%zu, n:%zu, u:%zu, b:%zu",
                        mesh->mPositions.size(),
                        mesh->mNormals.size(),
                        mesh->mTexcoords_0.size(),
                        mesh->mWeights_0.size());
            // @TODO: armature
        }
    });
}
