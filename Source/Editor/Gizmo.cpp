#include "Gizmo.h"

// #define IMGUI_DEFINE_MATH_OPERATORS
// #include <imgui/imgui_internal.h>
#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"

void gizmo_control(ImGuizmo::OPERATION op, const glm::mat4& view, const glm::mat4& proj, const Box2& rect, mat4& out)
{
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();

    const vec2& topLeft = rect.GetMin();
    const vec2 extent = rect.Size();

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(topLeft.x, topLeft.y, extent.x, extent.y);

    ImGuizmo::Manipulate(
        glm::value_ptr(view),
        glm::value_ptr(proj),
        op,
        ImGuizmo::WORLD,
        glm::value_ptr(out),
        nullptr, nullptr, nullptr, nullptr);
}

#include "ImGuizmo/ImGuizmo.cpp"

#if 0
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"

static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);

float objectMatrix[4][16] = {
    { 1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      0.f, 0.f, 0.f, 1.f },

    { 1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      2.f, 0.f, 0.f, 1.f },

    { 1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      2.f, 0.f, 2.f, 1.f },

    { 1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      0.f, 0.f, 2.f, 1.f }
};

static const float identityMatrix[16] = { 1.f, 0.f, 0.f, 0.f,
                                          0.f, 1.f, 0.f, 0.f,
                                          0.f, 0.f, 1.f, 0.f,
                                          0.f, 0.f, 0.f, 1.f };

void EditTransform(const float* view, const float* proj, float* matrix)
{
    static ImGuiWindowFlags gizmoWindowFlags = 0;

    ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
    ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
    ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
    ImGuizmo::SetDrawlist();
    float windowWidth = (float)ImGui::GetWindowWidth();
    float windowHeight = (float)ImGui::GetWindowHeight();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;

    ImGuizmo::DrawGrid(view, proj, identityMatrix, 100.f);
    ImGuizmo::DrawCubes(view, proj, &objectMatrix[0][0], 1);
    ImGuizmo::Manipulate(view, proj, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, nullptr, nullptr, nullptr, nullptr);

    ImGui::End();
}

void dummy_gizmo(mat4& view, mat4& proj)
{
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();

    ImGui::SetNextWindowPos(ImVec2(1024, 100), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(256, 256), ImGuiCond_Appearing);

    // create a window and insert the inspector
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(320, 340), ImGuiCond_Appearing);

    int matId = 0;
    ImGuizmo::SetID(matId);

    EditTransform(glm::value_ptr(view), glm::value_ptr(proj), objectMatrix[matId]);
}
#endif
