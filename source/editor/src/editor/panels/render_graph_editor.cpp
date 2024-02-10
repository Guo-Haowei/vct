#include "GraphEditor.h"
#include "servers/rendering/render_graph/render_graph.h"

extern vct::RenderGraph g_render_graph;

template<typename T, std::size_t N>
struct Array {
    T data[N];
    const size_t size() const { return N; }

    const T operator[](size_t index) const { return data[index]; }
    operator T*() {
        T* p = new T[N];
        memcpy(p, data, sizeof(data));
        return p;
    }
};

template<typename T, typename... U>
Array(T, U...) -> Array<T, 1 + sizeof...(U)>;

struct RenderGraphEditorDelegate : public GraphEditor::Delegate {
    bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override {
        return true;
    }

    void SelectNode(GraphEditor::NodeIndex nodeIndex, bool selected) override {
        mNodes[nodeIndex].mSelected = selected;
    }

    void MoveSelectedNodes(const ImVec2 delta) override {
        for (auto& node : mNodes) {
            if (!node.mSelected) {
                continue;
            }
            node.x += delta.x;
            node.y += delta.y;
        }
    }

    virtual void RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput) override {
    }

    void AddLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex) override {
        mLinks.push_back({ inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex });
    }

    void DelLink(GraphEditor::LinkIndex linkIndex) override {
        mLinks.erase(mLinks.begin() + linkIndex);
    }

    void CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex) override {
        vct::unused(drawList);
        vct::unused(rectangle);
        vct::unused(nodeIndex);
        // drawList->AddLine(rectangle.Min, rectangle.Max, IM_COL32(0, 0, 0, 255));
        // drawList->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), "Add your stuff here");
    }

    const size_t GetTemplateCount() override {
        return sizeof(mTemplates) / sizeof(GraphEditor::Template);
    }

    const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override {
        return mTemplates[index];
    }

    const size_t GetNodeCount() override {
        return mNodes.size();
    }

    const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override {
        const auto& myNode = mNodes[index];
        return GraphEditor::Node{
            myNode.name,
            myNode.templateIndex,
            ImRect(ImVec2(myNode.x, myNode.y), ImVec2(myNode.x + 200, myNode.y + 200)),
            myNode.mSelected
        };
    }

    const size_t GetLinkCount() override {
        return mLinks.size();
    }

    const GraphEditor::Link GetLink(GraphEditor::LinkIndex index) override {
        return mLinks[index];
    }

    // Graph datas
    static const inline GraphEditor::Template mTemplates[] = {
        {
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            1,
            Array{ "Input" },
            nullptr,
            1,
            Array{ "Output" },
            nullptr,
        }
    };

    struct Node {
        const char* name;
        GraphEditor::TemplateIndex templateIndex;
        float x, y;
        bool mSelected;
    };

    std::vector<Node> mNodes;
    std::vector<GraphEditor::Link> mLinks;
};

void dummy_graph_editor() {
    // Graph Editor
    static GraphEditor::Options options;
    static RenderGraphEditorDelegate delegate;
    static GraphEditor::ViewState viewState;
    static GraphEditor::FitOnScreen fit = GraphEditor::Fit_None;
    static bool showGraphEditor = true;

    if (delegate.mLinks.empty()) {
        for (const auto& pair : g_render_graph.m_links) {
            GraphEditor::Link link{
                .mInputNodeIndex = pair.first,
                .mInputSlotIndex = 0,
                .mOutputNodeIndex = pair.second,
                .mOutputSlotIndex = 0,
            };
            delegate.mLinks.emplace_back(link);
        }

        float offset = 0;
        for (int node_index : g_render_graph.m_sorted_order) {
            const std::shared_ptr<vct::RenderPass>& pass = g_render_graph.m_render_passes[node_index];
            delegate.mNodes.push_back(
                {
                    pass->get_name().c_str(),
                    0,
                    (200 * offset),
                    (200 * offset),
                    false,
                });
            ++offset;
        }
    }

    if (showGraphEditor) {
        ImGui::Begin("Graph Editor", NULL, 0);
        if (ImGui::Button("Fit all nodes")) {
            fit = GraphEditor::Fit_AllNodes;
        }
        ImGui::SameLine();
        if (ImGui::Button("Fit selected nodes")) {
            fit = GraphEditor::Fit_SelectedNodes;
        }
        GraphEditor::Show(delegate, options, viewState, true, &fit);

        ImGui::End();
    }
}
