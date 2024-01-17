#pragma once

#include "Math/Box.h"
#include "EditorLayer.h"

namespace ImGuizmo
{
enum OPERATION;
}

void gizmo_control(ImGuizmo::OPERATION op, const glm::mat4& view, const glm::mat4& proj, const Box2& rect, mat4& out);
