#include "GuiPass.hpp"

#include <filesystem>

#include "imgui/imgui.h"

#include "Base/Asserts.h"

#include "Core/com_dvars.h"

#include "Manager/SceneManager.hpp"

namespace fs = std::filesystem;

static void DrawDebugWindow()
{
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

        ImGui::Separator();
        ImGui::Text( "Light" );
        float* lightDir = (float*)Dvar_GetPtr( light_dir );
        dirty |= ImGui::SliderFloat( "x", lightDir, -20.f, 20.f );
        dirty |= ImGui::SliderFloat( "z", lightDir + 2, -20.f, 20.f );

        scene.light.direction = glm::normalize( Dvar_GetVec3( light_dir ) );
        scene.dirty = dirty;
    }
    ImGui::End();
}

static void DrawEntity( Entity* entity )
{
    if ( ImGui::TreeNode( entity->m_name.c_str() ) ) {
        if ( entity->m_flag & Entity::FLAG_GEOMETRY ) {
            //MeshComponent& mesh = *entity->m_mesh;
            MaterialComponent& mat = *entity->m_material;
            auto materialTextures = mat.gpuResource;

            constexpr float scale = 0.35f;
            ImVec2 imageSize = ImGui::GetWindowSize();
            imageSize.x *= scale;
            imageSize.y *= scale;
            if ( !mat.albedoTexture.empty() ) {
                auto name = fs::path( mat.albedoTexture ).filename();
                ImGui::Text( "Albedo: %s", name.string().c_str() );

                ImGui::Image( (ImTextureID)materialTextures->albedoMap.handle, imageSize, ImVec2( 0, 1 ), ImVec2( 1, 0 ) );
            }
            if ( !mat.normalTexture.empty() ) {
                auto name = fs::path( mat.normalTexture ).filename();
                ImGui::Text( "Normal: %s", name.string().c_str() );

                ImGui::Image( (ImTextureID)materialTextures->normalMap.handle, imageSize, ImVec2( 0, 1 ), ImVec2( 1, 0 ) );
            }
            if ( !mat.pbrTexture.empty() ) {
                auto name = fs::path( mat.pbrTexture ).filename();
                ImGui::Text( "PBR: %s", name.string().c_str() );

                ImGui::Image( (ImTextureID)materialTextures->pbrMap.handle, imageSize, ImVec2( 0, 1 ), ImVec2( 1, 0 ) );
            }
        }

        for ( Entity* child : entity->m_children ) {
            DrawEntity( child );
        }
        ImGui::TreePop();
    }
}

static void DrawSceneHierachy()
{
    ImGui::Begin( "Scene Hierarchy" );
    Scene& scene = Com_GetScene();
    const uint32_t numMeshes = uint32_t( scene.m_meshes.size() );
    const uint32_t numMaterials = uint32_t( scene.m_materials.size() );
    ImGui::Text( "scene has %u meshes, %u materials", numMeshes, numMaterials );
    ImGui::Separator();
    Entity* root = scene.m_root;
    if ( root ) {
        DrawEntity( root );
    }
    ImGui::End();
}

void GuiPass::Draw( Frame& frame )
{
    unused( frame );
    DrawDebugWindow();
    DrawSceneHierachy();
}