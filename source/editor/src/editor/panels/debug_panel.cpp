#include "debug_panel.h"

#include "core/framework/common_dvars.h"
#include "rendering/r_defines.h"
#include "rendering/rendering_dvars.h"
#include "scene/scene.h"

namespace vct {

void DebugPanel::update_internal(Scene&) {
    ImGui::Text("Voxel GI");
    ImGui::Checkbox("Enable GI", (bool*)(DVAR_GET_POINTER(r_enable_vxgi)));
    ImGui::Checkbox("No Texture", (bool*)(DVAR_GET_POINTER(r_no_texture)));
    int debug_texture = DVAR_GET_INT(r_debug_texture);

    constexpr float offset = 150;
    ImGui::RadioButton("VXGI", &debug_texture, 0);
    ImGui::SameLine(ImGui::GetWindowWidth() - offset);
    ImGui::RadioButton("Depth Map", &debug_texture, TEXTURE_GBUFFER_DEPTH);

    ImGui::RadioButton("Voxel: Color", &debug_texture, TEXTURE_VOXEL_ALBEDO);
    ImGui::SameLine(ImGui::GetWindowWidth() - offset);
    ImGui::RadioButton("Voxel: Normal", &debug_texture, TEXTURE_VOXEL_NORMAL);

    ImGui::RadioButton("Gbuffer: Albedo", &debug_texture, TEXTURE_GBUFFER_ALBEDO);
    ImGui::SameLine(ImGui::GetWindowWidth() - offset);
    ImGui::RadioButton("Gbuffer: Normal", &debug_texture, TEXTURE_GBUFFER_NORMAL);

    ImGui::RadioButton("Gbuffer: Roughness", &debug_texture, TEXTURE_GBUFFER_ROUGHNESS);
    ImGui::SameLine(ImGui::GetWindowWidth() - offset);
    ImGui::RadioButton("Gbuffer: Metallic ", &debug_texture, TEXTURE_GBUFFER_METALLIC);

    ImGui::RadioButton("SSAO Map", &debug_texture, TEXTURE_SSAO);
    ImGui::SameLine(ImGui::GetWindowWidth() - offset);
    ImGui::RadioButton("Shadow Map", &debug_texture, TEXTURE_SHADOW_MAP);

    DVAR_SET_INT(r_debug_texture, debug_texture);
    ImGui::Separator();

    ImGui::DragFloat("World size", (float*)DVAR_GET_POINTER(r_world_size), 1.0f, 1.0f, 100.0f);
    ImGui::Separator();

    ImGui::Text("SSAO");
    ImGui::Checkbox("Enable SSAO", (bool*)(DVAR_GET_POINTER(r_enableSsao)));
    ImGui::Text("SSAO Kernal Radius");
    ImGui::SliderFloat("Kernal Radius", (float*)(DVAR_GET_POINTER(r_ssaoKernelRadius)), 0.1f, 5.0f);
    ImGui::Separator();

    ImGui::Text("FXAA");
    ImGui::Checkbox("Enable FXAA", (bool*)(DVAR_GET_POINTER(r_enableFXAA)));
    ImGui::Separator();

    ImGui::Checkbox("toggle grid visibility", (bool*)(DVAR_GET_POINTER(grid_visibility)));
}

}  // namespace vct
