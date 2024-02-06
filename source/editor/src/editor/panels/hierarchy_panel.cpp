#include "hierarchy_panel.h"

#include "imgui/imgui_internal.h"
#include "scene/scene.h"

class HierarchyCreator {
public:
    struct HierarchyNode {
        HierarchyNode* parent = nullptr;
        ecs::Entity entity;

        std::vector<HierarchyNode*> children;
    };

    HierarchyCreator(HierarchyPanel& panel) : mPanel(panel) {}

    void Draw(const Scene& scene) {
        if (Build(scene)) {
            DEV_ASSERT(mRoot);
            DrawNode(scene, mRoot, ImGuiTreeNodeFlags_DefaultOpen);
        }
    }

private:
    bool Build(const Scene& scene);
    void DrawNode(const Scene& scene, HierarchyNode* pNode, ImGuiTreeNodeFlags flags = 0);

    std::map<ecs::Entity, std::shared_ptr<HierarchyNode>> m_nodes;
    HierarchyNode* mRoot = nullptr;
    HierarchyPanel& mPanel;
};

void HierarchyCreator::DrawNode(const Scene& scene, HierarchyNode* pHier, ImGuiTreeNodeFlags flags) {
    DEV_ASSERT(pHier);
    ecs::Entity id = pHier->entity;
    const TagComponent* tagComponent = scene.get_component<TagComponent>(id);
    const char* name = tagComponent ? tagComponent->GetTag().c_str() : "Untitled";

    auto nodeTag = std::format("##{}", id.GetID());
    auto tag = std::format("{}{}", name, nodeTag);

    flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
    flags |= pHier->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0;
    flags |= mPanel.GetSelected() == id ? ImGuiTreeNodeFlags_Selected : 0;
    bool expanded = ImGui::TreeNodeEx(nodeTag.c_str(), flags);
    ImGui::SameLine();
    if (ImGui::Selectable(tag.c_str())) {
        mPanel.SetSelected(id);
    }
    if (expanded) {
        float indentWidth = 8.f;
        ImGui::Indent(indentWidth);
        for (auto& child : pHier->children) {
            DrawNode(scene, child);
        }
        ImGui::Unindent(indentWidth);
    }
}

bool HierarchyCreator::Build(const Scene& scene) {
    const size_t hierarchy_count = scene.get_count<HierarchyComponent>();
    if (hierarchy_count == 0) {
        return false;
    }

    for (int i = 0; i < hierarchy_count; ++i) {
        auto FindOrCreate = [this](ecs::Entity id) {
            auto it = m_nodes.find(id);
            if (it == m_nodes.end()) {
                m_nodes[id] = std::make_shared<HierarchyNode>();
                return m_nodes[id].get();
            }
            return it->second.get();
        };

        const HierarchyComponent& hier = scene.get_component<HierarchyComponent>(i);
        const ecs::Entity selfId = scene.get_entity<HierarchyComponent>(i);
        const ecs::Entity parentId = hier.GetParent();
        HierarchyNode* parentNode = FindOrCreate(parentId);
        HierarchyNode* selfNode = FindOrCreate(selfId);
        parentNode->children.push_back(selfNode);
        parentNode->entity = parentId;
        selfNode->parent = parentNode;
        selfNode->entity = selfId;
    }

    int nodes_without_parent = 0;
    for (auto& it : m_nodes) {
        if (!it.second->parent) {
            ++nodes_without_parent;
            mRoot = it.second.get();
        }
    }
    DEV_ASSERT(nodes_without_parent == 1);
    return true;
}

void HierarchyPanel::RenderInternal(Scene& scene) {
    HierarchyCreator creator(*this);
    creator.Draw(scene);
}
