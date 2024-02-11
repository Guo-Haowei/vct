#include "animation_panel.h"

#include "imgui/imgui_internal.h"
#include "scene/scene.h"

void AnimationPanel::RenderInternal(Scene& scene) {
    if (ImGui::Begin("Animation")) {
        for (int i = 0; i < scene.get_count<AnimationComponent>(); ++i) {
            ecs::Entity id = scene.get_entity<AnimationComponent>(i);
            AnimationComponent& animation = scene.get_component_array<AnimationComponent>()[i];
            TagComponent* tag = scene.get_component<TagComponent>(id);
            ImGui::Text("Animation %s", tag->get_tag().c_str());
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
    ImGui::End();
}
