#include "debug_panel.h"

#include "Engine/Core/camera.h"
#include "scene/scene.h"
#include "servers/rendering/r_defines.h"

/////////////////////
#include "core/dynamic_variable/common_dvars.h"

static const char* DrawTextureToStr(int mode) {
    const char* str = "scene";
    switch (mode) {
        case TEXTURE_VOXEL_ALBEDO:
            str = "voxel color";
            break;
        case TEXTURE_VOXEL_NORMAL:
            str = "voxel normal";
            break;
        case TEXTURE_GBUFFER_DEPTH:
            str = "depth";
            break;
        case TEXTURE_GBUFFER_ALBEDO:
            str = "albedo";
            break;
        case TEXTURE_GBUFFER_NORMAL:
            str = "normal";
            break;
        case TEXTURE_GBUFFER_METALLIC:
            str = "metallic";
            break;
        case TEXTURE_GBUFFER_ROUGHNESS:
            str = "roughness";
            break;
        case TEXTURE_GBUFFER_SHADOW:
            str = "shadow";
            break;
        case TEXTURE_SSAO:
            str = "ssao";
            break;
        default:
            break;
    }
    return str;
}

void DebugPanel::RenderInternal(Scene&) {
    const Camera& camera = gCamera;
    bool dirty = false;

    const vec3& eye = camera.position;
    ImGui::Text("eye: %.2f, %.2f, %.2f", eye.x, eye.y, eye.z);
    ImGui::Separator();

    ImGui::Text("Voxel GI");
    ImGui::Checkbox("Enable GI", (bool*)(DVAR_GET_POINTER(r_enableVXGI)));
    ImGui::Checkbox("No Texture", (bool*)(DVAR_GET_POINTER(r_noTexture)));
    dirty |= ImGui::Checkbox("Force Voxel GI texture update", (bool*)(DVAR_GET_POINTER(r_forceVXGI)));
    ImGui::Separator();

    ImGui::Text("CSM");
    ImGui::Checkbox("Debug CSM", (bool*)(DVAR_GET_POINTER(r_debugCSM)));
    ImGui::Separator();

    ImGui::Text("SSAO");
    ImGui::Checkbox("Enable SSAO", (bool*)(DVAR_GET_POINTER(r_enableSsao)));
    ImGui::Text("SSAO Kernal Radius");
    ImGui::SliderFloat("Kernal Radius", (float*)(DVAR_GET_POINTER(r_ssaoKernelRadius)), 0.1f, 5.0f);
    ImGui::Separator();

    ImGui::Text("FXAA");
    ImGui::Checkbox("Enable FXAA", (bool*)(DVAR_GET_POINTER(r_enableFXAA)));
    ImGui::Separator();

    ImGui::Text("Display Texture");
    ImGui::SliderInt("Display Texture", (int*)(DVAR_GET_POINTER(r_debugTexture)), DrawTexture::TEXTURE_FINAL_IMAGE,
                     DrawTexture::TEXTURE_MAX);
    ImGui::Text("%s", DrawTextureToStr(DVAR_GET_INT(r_debugTexture)));
}
