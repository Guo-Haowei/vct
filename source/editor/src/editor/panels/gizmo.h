#pragma once

#include "core/math/box.h"

namespace ImGuizmo {
enum OPERATION : int;
}

void gizmo_control(ImGuizmo::OPERATION op, const glm::mat4& view, const glm::mat4& proj, const vct::Box2& rect, mat4& out);
