#include "panel.h"

#include "core/input/input.h"
#include "imgui/imgui_internal.h"

namespace vct {

void Panel::update(Scene& scene) {
    if (ImGui::Begin(m_name.c_str())) {
        update_internal(scene);
    }
    ImGui::End();
}

}  // namespace vct
