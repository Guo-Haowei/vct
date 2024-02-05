#include "gizmo.h"

#include "imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGuizmo/ImGuizmo.h"
#include "imgui/imgui_internal.h"

void gizmo_control(ImGuizmo::OPERATION op, const glm::mat4& view, const glm::mat4& proj, const vct::Box2& rect, mat4& out) {
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();

    const vec2& topLeft = rect.get_min();
    const vec2 extent = rect.size();

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(topLeft.x, topLeft.y, extent.x, extent.y);

    ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), op, ImGuizmo::WORLD, glm::value_ptr(out), nullptr,
                         nullptr, nullptr, nullptr);
}

#include "ImGuizmo/ImGuizmo.cpp"
