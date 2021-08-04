#include "com_cmdline.h"
#include "com_filesystem.h"
#include "com_system.h"

// include here
#include <GLFW/glfw3.h>

#include <iostream>

#include "Globals.h"
#include "Window.h"
#include "com_system.h"
#include "imgui/imgui.h"
#include "renderer/RenderSystem.h"
#include "universal/core_math.h"
#include "universal/print.h"

using namespace vct;
Window m_window;
RenderSystem m_renderSystem;

int run();

int main( int argc, const char** argv )
{
    bool ok = true;

    ok = ok && Com_FsInit();
    ok = ok && Com_RegisterDvars();
    ok = ok && Com_ProcessCmdLine( argc - 1, argv + 1 );
    ok = ok && Com_LoadScene();
    ok = ok && Com_InitMainWindow();

    m_window.initialize();
    m_renderSystem.initialize( &m_window );

    run();

    m_renderSystem.finalize();
    ImGui::DestroyContext();
    m_window.finalize();

    Com_DestroyMainWindow();

    return ok;
}

void updateCamera()
{
    Scene& scene = Com_GetScene();

    constexpr float VIEW_SPEED = 2.0f;
    float CAMERA_SPEED         = 0.15f;

    if ( m_window.isKeyDown( GLFW_KEY_LEFT_SHIFT ) )
        CAMERA_SPEED *= 3.f;

    int x = m_window.isKeyDown( GLFW_KEY_D ) - m_window.isKeyDown( GLFW_KEY_A );
    int z = m_window.isKeyDown( GLFW_KEY_W ) - m_window.isKeyDown( GLFW_KEY_S );
    int y = m_window.isKeyDown( GLFW_KEY_E ) - m_window.isKeyDown( GLFW_KEY_Q );

    Camera& cam = scene.camera;
    if ( x != 0 || z != 0 )
    {
        vec3 w           = cam.direction();
        vec3 u           = glm::cross( w, vec3( 0, 1, 0 ) );
        vec3 translation = ( CAMERA_SPEED * z ) * w + ( CAMERA_SPEED * x ) * u;
        cam.position += translation;
    }

    cam.position.y += ( CAMERA_SPEED * y );

    int yaw   = m_window.isKeyDown( GLFW_KEY_RIGHT ) - m_window.isKeyDown( GLFW_KEY_LEFT );
    int pitch = m_window.isKeyDown( GLFW_KEY_UP ) - m_window.isKeyDown( GLFW_KEY_DOWN );

    if ( yaw )
        cam.yaw += VIEW_SPEED * yaw;

    if ( pitch )
    {
        cam.pitch += VIEW_SPEED * pitch;
        cam.pitch = glm::clamp( cam.pitch, -80.0f, 80.0f );
    }
}

UIControlls g_UIControls;

void userInterface()
{
    Scene& scene = Com_GetScene();

    ImGui::NewFrame();

    if ( ImGui::BeginMainMenuBar() )
    {
        if ( ImGui::BeginMenu( "Debug" ) )
        {
            ImGui::Text( "Voxel GI" );
            ImGui::SameLine();
            ImGui::RadioButton( "On", &g_UIControls.voxelGiMode, 1 );
            ImGui::SameLine();
            ImGui::RadioButton( "Off", &g_UIControls.voxelGiMode, 0 );
            ImGui::Separator();

            ImGui::Text( "Voxel Texture Size: %d", VOXEL_TEXTURE_SIZE );
            ImGui::Text( "Voxel Mip Level: %d", VOXEL_TEXTURE_MIP_LEVEL );
            if ( ImGui::Checkbox( "Force voxel texture update", &g_UIControls.forceUpdateVoxelTexture ) )
                scene.dirty = true;
            ImGui::Separator();
            ImGui::SliderInt( "Voxel Mipmap Level", &g_UIControls.voxelMipLevel, 0, VOXEL_TEXTURE_MIP_LEVEL - 1 );
            ImGui::Separator();

            ImGui::Text( "Debug Bounding Box" );
            if ( ImGui::Checkbox( "Show Object Bounding Box", &g_UIControls.showObjectBoundingBox ) )
                g_UIControls.showWorldBoundingBox = false;
            if ( ImGui::Checkbox( "Show World Bounding Box", &g_UIControls.showWorldBoundingBox ) )
                g_UIControls.showObjectBoundingBox = false;

            ImGui::Separator();

            ImGui::Text( "Debug Texture" );
            ImGui::RadioButton( "Voxel GI", &g_UIControls.drawTexture, DrawTexture::TEXTURE_FINAL_IMAGE );
            ImGui::RadioButton( "Voxel Albedo", &g_UIControls.drawTexture, DrawTexture::TEXTURE_VOXEL_ALBEDO );
            ImGui::RadioButton( "Voxel Normal", &g_UIControls.drawTexture, DrawTexture::TEXTURE_VOXEL_NORMAL );
            ImGui::RadioButton( "Gbuffer Depth", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_DEPTH );
            ImGui::RadioButton( "Gbuffer Albedo", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_ALBEDO );
            ImGui::RadioButton( "Gbuffer Normal", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_NORMAL );
            ImGui::RadioButton( "Gbuffer Metallic", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_METALLIC );
            ImGui::RadioButton( "Gbuffer Roughness", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_ROUGHNESS );
            ImGui::RadioButton( "Gbuffer Shadow", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_SHADOW );

            ImGui::EndMenu();
        }

        ImGui::Separator();

        if ( ImGui::BeginMenu( "Light" ) )
        {
            scene.lightDirty |= ImGui::SliderFloat( "position x", &scene.light.position.x, -10.0f, 10.0f );
            scene.lightDirty |= ImGui::SliderFloat( "position y", &scene.light.position.y, 20.0f, 30.0f );
            scene.lightDirty |= ImGui::SliderFloat( "position z", &scene.light.position.z, -10.0f, 10.0f );
            scene.dirty |= scene.lightDirty;
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if ( ImGui::BeginMenu( "Log" ) )
        {
            ImGui::Text( "FPS            : %.2f FPS", ImGui::GetIO().Framerate );
            ImGui::Text( "Total meshes   : %.d", g_UIControls.totalMeshes );
            ImGui::Text( "Total Materials: %.d", g_UIControls.totalMaterials );
            ImGui::Text( "Object occluded: %.d", g_UIControls.objectOccluded );
            ImGui::EndMenu();
        }

        ImGui::Separator();

        ImGui::EndMenuBar();
    }

    ImGui::End();

    // Rendering
    ImGui::Render();
}

int run()
{
    Scene& scene = Com_GetScene();

    while ( !m_window.shouldClose() )
    {
        m_window.pollEvents();
        m_window.beginFrame();

        updateCamera();

        int w, h;
        m_window.getFrameExtent( &w, &h );
        float aspect        = (float)w / h;
        scene.camera.aspect = aspect;

        userInterface();

        g_UIControls.objectOccluded = 0;

        m_renderSystem.update();

        m_window.swapBuffers();

        scene.dirty      = false;
        scene.lightDirty = false;
    }

    return 0;
}
