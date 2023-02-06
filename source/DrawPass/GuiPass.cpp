#include "GuiPass.hpp"

#include "imgui/imgui.h"

#include "Core/WindowManager.h"
#include "Core/com_dvars.h"
#include "Core/com_misc.h"
#include "Graphics/gl_utils.h"

void GuiPass::Draw( Frame& frame )
{
    unused( frame );
    if ( ImGui::Begin( "Debug" ) ) {
        Scene& scene = Com_GetScene();
        const Camera& camera = scene.camera;
        bool dirty = false;

        const vec3& eye = camera.position;
        ImGui::Text( "Eye position: %.2f, %.2f, %.2f", eye.x, eye.y, eye.z );
        ImGui::Separator();

        ImGui::Checkbox( "Show debug textures", (bool*)( Dvar_GetPtr( r_showDebugTexture ) ) );
        dirty |= ImGui::Checkbox( "Force Voxel GI texture update", (bool*)( Dvar_GetPtr( r_forceVXGI ) ) );
        ImGui::Checkbox( "Enable Voxel GI", (bool*)( Dvar_GetPtr( r_enableVXGI ) ) );
        ImGui::Checkbox( "Default mesh", (bool*)( Dvar_GetPtr( r_noTexture ) ) );
        ImGui::Separator();

        ImGui::Text( "SSAO" );
        ImGui::Checkbox( "Enable SSAO", (bool*)( Dvar_GetPtr( r_enableSsao ) ) );
        ImGui::Text( "SSAO Kernal Radius" );
        ImGui::SliderFloat( "Kernal Radius", (float*)( Dvar_GetPtr( r_ssaoKernelRadius ) ), 0.1f, 5.0f );
        ImGui::Separator();

        ImGui::Separator();
        ImGui::Text( "Light" );
        float* lightDir = (float*)Dvar_GetPtr( light_dir );
        dirty |= ImGui::SliderFloat( "x", lightDir, -20.f, 20.f );
        dirty |= ImGui::SliderFloat( "z", lightDir + 2, -20.f, 20.f );

        ImGui::Text( "Floor" );
        if ( scene.selected ) {
            void* mat = scene.selected->material->gpuResource;
            MaterialData* drawData = reinterpret_cast<MaterialData*>( mat );
            dirty |= ImGui::SliderFloat( "metallic", &drawData->metallic, 0.0f, 1.0f );
            dirty |= ImGui::SliderFloat( "roughness", &drawData->roughness, 0.0f, 1.0f );
        }

        scene.light.direction = glm::normalize( Dvar_GetVec3( light_dir ) );
        scene.dirty = dirty;
    }
    ImGui::End();
}