#include "animation_panel.h"

#include "imgui/imgui_internal.h"
#include "scene/scene.h"

namespace vct {

void AnimationPanel::update_internal(Scene& scene) {
    for (int i = 0; i < scene.get_count<AnimationComponent>(); ++i) {
        ecs::Entity id = scene.get_entity<AnimationComponent>(i);
        AnimationComponent& animation = scene.get_component_array<AnimationComponent>()[i];
        NameComponent* name = scene.get_component<NameComponent>(id);
        ImGui::Text("Animation %s", name->get_name().c_str());
        if (!animation.is_playing()) {
            if (ImGui::Button("play")) {
                animation.flags |= AnimationComponent::PLAYING;
            }
        } else {
            if (ImGui::Button("stop")) {
                animation.flags &= ~AnimationComponent::PLAYING;
            }
        }
        if (ImGui::SliderFloat("Frame", &animation.timer, animation.start, animation.end)) {
            animation.flags |= AnimationComponent::PLAYING;
        }
        ImGui::Separator();
    }
}

}  // namespace vct
