#include "GraphicsManager.hpp"

#include "Base/Asserts.h"

#include "Core/com_dvars.h"
#include "Core/com_misc.h"
#include "Core/WindowManager.h"

#ifdef max
#undef max
#endif

void GraphicsManager::Tick()
{
    UpdateConstants();

    // BeginFrame( m_Frames[m_nFrameIndex] );
    // ImGui::NewFrame();
    // Draw();
    // ImGui::EndFrame();
    // ImGui::Render();
    // EndFrame( m_Frames[m_nFrameIndex] );

    // Present();

    // ImGuiIO& io = ImGui::GetIO();
    // if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable ) {
    //     ImGui::UpdatePlatformWindows();
    //     ImGui::RenderPlatformWindowsDefault();
    // }
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
    const vec3 center = scene.boundingBox.Center();
    const vec3 extents = scene.boundingBox.Size();
    const float size = 0.5f * glm::max( extents.x, glm::max( extents.y, extents.z ) );
    const mat4 V = glm::lookAt( center + glm::normalize( lightDir ) * size, center, vec3( 0, 1, 0 ) );
    const mat4 P = glm::ortho( -size, size, -size, size, 0.0f, 2.0f * size );
    return P * V;
}

void GraphicsManager::UpdateConstants()
{
    const Scene& scene = Com_GetScene();
    PerFrameConstants& frameConstats = m_frame.frameContexts;

    // validate dvars
    const int voxelTextureSize = Dvar_GetInt( r_voxelSize );
    ASSERT( is_power_of_two( voxelTextureSize ) );
    ASSERT( voxelTextureSize <= 256 );
    const vec3 center = scene.boundingBox.Center();
    const vec3 size = scene.boundingBox.Size();
    const float worldSize = glm::max( size.x, glm::max( size.y, size.z ) );
    const float texelSize = 1.0f / static_cast<float>( voxelTextureSize );
    const float voxelSize = worldSize * texelSize;

    // HACK
    static bool s_tmp = false;
    if ( !s_tmp ) {
        frameConstats.WorldCenter = center;
        frameConstats.WorldSizeHalf = 0.5f * worldSize;
        frameConstats.TexelSize = texelSize;
        frameConstats.VoxelSize = voxelSize;
        frameConstats.LightPV = R_HackLightSpaceMatrix( scene.light.direction );
        s_tmp = true;
    }

    const Camera& camera = scene.camera;
    const ivec2 extent = g_wndMgr->FrameSize();

    // update constants
    frameConstats.SunDir = scene.light.direction;
    frameConstats.LightColor = scene.light.color;
    frameConstats.CamPos = camera.position;
    frameConstats.View = camera.View();
    frameConstats.Proj = camera.Proj();
    frameConstats.EnableGI = Dvar_GetBool( r_enableVXGI );
    frameConstats.DebugCSM = Dvar_GetBool( r_debugCSM );
    frameConstats.DebugTexture = Dvar_GetInt( r_debugTexture );
    frameConstats.NoTexture = Dvar_GetBool( r_noTexture );
    frameConstats.ScreenWidth = extent.x;
    frameConstats.ScreenHeight = extent.y;

    // SSAO
    frameConstats.SSAOKernelSize = Dvar_GetInt( r_ssaoKernelSize );
    frameConstats.SSAOKernelRadius = Dvar_GetFloat( r_ssaoKernelRadius );
    frameConstats.SSAONoiseSize = Dvar_GetInt( r_ssaoNoiseSize );
    frameConstats.EnableSSAO = Dvar_GetBool( r_enableSsao );

    // FXAA
    frameConstats.EnableFXAA = Dvar_GetBool( r_enableFXAA );
}