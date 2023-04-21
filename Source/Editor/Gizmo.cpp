#include "Gizmo.h"

// #define IMGUI_DEFINE_MATH_OPERATORS
// #include <imgui/imgui_internal.h>
#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

// void Gizmo(ImGuizmo::OPERATION op, const glm::mat4& view, const glm::mat4& proj, const Viewport& viewport, mat4& out)
// {
//     ImGuizmo::SetOrthographic(false);
//     ImGuizmo::BeginFrame();

//     const float x = viewport.topLeftX + viewport.windowX;
//     const float y = viewport.topLeftY + viewport.windowY;
//     ImGuizmo::SetRect(x, y, viewport.width, viewport.height);

//     ImGuizmo::Manipulate(
//         glm::value_ptr(view),
//         glm::value_ptr(proj),
//         op,
//         ImGuizmo::WORLD,
//         glm::value_ptr(out),
//         nullptr, nullptr, nullptr, nullptr);
// }

#include "ImGuizmo/ImGuizmo.cpp"
