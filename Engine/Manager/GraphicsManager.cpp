#include "GraphicsManager.hpp"

#include "Base/Asserts.h"

#include "Core/com_dvars.h"

#include "imgui/imgui.h"

#include "Interface/IApplication.hpp"
#include "Manager/SceneManager.hpp"

#ifdef max
#undef max
#endif

bool GraphicsManager::Initialize()
{
    return true;
}

void GraphicsManager::Tick()
{
    UpdateConstants();

    // Frame& frame = m_frames[];

    Frame& frame = m_frame;
    BeginFrame( frame );
    ImGui::NewFrame();

    Draw();

    ImGui::EndFrame();
    ImGui::Render();
    EndFrame( frame );

    Present();
}

void GraphicsManager::Draw()
{
    Frame& frame = m_frame;

    for ( auto& drawPass : m_drawPasses ) {
        drawPass->BeginPass( frame );
        drawPass->Draw( frame );
        drawPass->EndPass( frame );
    }
}

void GraphicsManager::CalculateCameraMatrix()
{
}

void GraphicsManager::CalculateLights()
{
}

static mat4 R_HackLightSpaceMatrix( const vec3& lightDir )
{
    const Scene& scene = Com_GetScene();
    const vec3 center = scene.m_aabb.Center();
    const vec3 extents = scene.m_aabb.Size();
    const float size = 0.5f * glm::max( extents.x, glm::max( extents.y, extents.z ) );
    const mat4 V = glm::lookAt( center + glm::normalize( lightDir ) * size, center, vec3( 0, 1, 0 ) );
    const mat4 P = glm::ortho( -size, size, -size, size, 0.0f, 2.0f * size );
    return P * V;
}

void GraphicsManager::UpdateConstants()
{
    const Scene& scene = Com_GetScene();

    for ( auto& dbc : m_frame.batchContexts ) {
        const Entity* entity = dbc->pEntity;
        entity->GetCalculatedTransform( dbc->Model );
        const MaterialComponent* mat = entity->m_material;
        const MaterialTextures* textures = reinterpret_cast<const MaterialTextures*>( mat->gpuResource );
        dbc->AlbedoColor = mat->albedoColor;
        dbc->Metallic = mat->metallic;
        dbc->Roughness = mat->roughness;
        dbc->HasAlbedoMap = textures->albedoMap.handle ? 1.0f : 0.0f;
        dbc->HasNormalMap = textures->normalMap.handle ? 1.0f : 0.0f;
        dbc->HasPbrMap = textures->pbrMap.handle ? 1.0f : 0.0f;
        dbc->ReflectPower = mat->reflectPower;
    }

    PerFrameConstants& frameConstats = m_frame.frameContexts;

    // validate dvars
    const int voxelTextureSize = Dvar_GetInt( r_voxelSize );
    ASSERT( is_power_of_two( voxelTextureSize ) );
    ASSERT( voxelTextureSize <= 256 );
    const vec3 center = scene.m_aabb.Center();
    const vec3 size = scene.m_aabb.Size();
    const float worldSize = glm::max( size.x, glm::max( size.y, size.z ) );
    const float texelSize = 1.0f / static_cast<float>( voxelTextureSize );
    const float voxelSize = worldSize * texelSize;

    frameConstats.WorldCenter = center;
    frameConstats.WorldSizeHalf = 0.5f * worldSize;
    frameConstats.TexelSize = texelSize;
    frameConstats.VoxelSize = voxelSize;
    frameConstats.LightPV = R_HackLightSpaceMatrix( scene.light.direction );

    const Camera& camera = scene.camera;
    int w, h;
    m_pApp->GetFramebufferSize( w, h );

    // update constants
    frameConstats.SunDir = scene.light.direction;
    frameConstats.LightColor = scene.light.color;
    frameConstats.CamPos = camera.position;
    frameConstats.View = camera.View();
    frameConstats.Proj = camera.Proj();
    frameConstats.EnableGI = Dvar_GetBool( r_enableVXGI );
    frameConstats.NoTexture = Dvar_GetBool( r_noTexture );
    frameConstats.ScreenWidth = w;
    frameConstats.ScreenHeight = h;
}