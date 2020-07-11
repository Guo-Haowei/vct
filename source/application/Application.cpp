#include "Application.h"
#include "base/Exception.h"
#include "imgui/imgui.h"
#include "scene/CreateScene.h"
#include "Globals.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace vct {

int Application::run()
{
    try
    {
        initialize();
        while (!m_window.shouldClose())
        {
            m_window.pollEvents();
            m_window.beginFrame();

            updateCamera();

            auto extent = m_window.getFrameExtent();
            float aspect = (float)extent.witdh / (float)extent.height;
            g_scene.camera.aspect = aspect;

            userInterface();

            g_UIControls.objectOccluded = 0;

            m_renderSystem.update();

            m_window.swapBuffers();

            g_scene.dirty = false;
            g_scene.lightDirty = false;
        }

        finalize();

        return 0;
    }
    catch(const Exception& e)
    {
        std::cerr << e.what() << std::endl;

        return -1;
    }
}

void Application::commandLineArgs(int argc, const char** argv)
{
}

void Application::initialize()
{
    // load scene
    createDefaultScene();

    m_window.initialize();
    m_renderSystem.initialize(&m_window);
}

void Application::finalize()
{
    m_renderSystem.finalize();
    ImGui::DestroyContext();
    m_window.finalize();
}

void Application::updateCamera()
{
    constexpr float VIEW_SPEED = 2.0f;
    float CAMERA_SPEED = 0.2f;

    if (m_window.isKeyDown(GLFW_KEY_LEFT_SHIFT))
        CAMERA_SPEED *= 3.f;

    int x = m_window.isKeyDown(GLFW_KEY_D) - m_window.isKeyDown(GLFW_KEY_A);
    int z = m_window.isKeyDown(GLFW_KEY_W) - m_window.isKeyDown(GLFW_KEY_S);
    int y = m_window.isKeyDown(GLFW_KEY_E) - m_window.isKeyDown(GLFW_KEY_Q);

    Camera& cam = g_scene.camera;
    if (x != 0 || z != 0)
    {
        Vector3 w = cam.direction();
        Vector3 u = three::cross(w, Vector3::UnitY);
        Vector3 translation = (CAMERA_SPEED * z) * w + (CAMERA_SPEED * x) * u;
        cam.position += translation;
    }

    cam.position.y += (CAMERA_SPEED * y);

    int yaw = m_window.isKeyDown(GLFW_KEY_RIGHT) - m_window.isKeyDown(GLFW_KEY_LEFT);
    int pitch = m_window.isKeyDown(GLFW_KEY_UP) - m_window.isKeyDown(GLFW_KEY_DOWN);


    if (yaw)
        cam.yaw += VIEW_SPEED * yaw;

    if (pitch)
    {
        cam.pitch += VIEW_SPEED * pitch;
        cam.pitch = three::clamp(cam.pitch, -80.0f, 80.0f);
    }
}

void Application::userInterface()
{
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Debug"))
        {
            ImGui::Text("Voxel GI"); ImGui::SameLine();
            ImGui::RadioButton("On", &g_UIControls.voxelGiMode, 1); ImGui::SameLine();
            ImGui::RadioButton("Off", &g_UIControls.voxelGiMode, 0);
            ImGui::Separator();

            ImGui::Text("Voxel Texture Size: %d", VOXEL_TEXTURE_SIZE);
            ImGui::Text("Voxel Mip Level: %d", VOXEL_TEXTURE_MIP_LEVEL);
            if (ImGui::Checkbox("Force voxel texture update", &g_UIControls.forceUpdateVoxelTexture))
                g_scene.dirty = true;
            ImGui::Separator();
            ImGui::SliderInt("Voxel Mipmap Level", &g_UIControls.voxelMipLevel, 0, VOXEL_TEXTURE_MIP_LEVEL - 1);
            ImGui::Separator();

            ImGui::Text("Debug Bounding Box");
            if (ImGui::Checkbox("Show Object Bounding Box", &g_UIControls.showObjectBoundingBox))
                g_UIControls.showWorldBoundingBox = false;
            if (ImGui::Checkbox("Show World Bounding Box", &g_UIControls.showWorldBoundingBox))
                g_UIControls.showObjectBoundingBox = false;

            ImGui::Separator();

            ImGui::Text("Debug Texture");
            ImGui::RadioButton("Voxel GI",          &g_UIControls.drawTexture, DrawTexture::TEXTURE_FINAL_IMAGE);
            ImGui::RadioButton("Voxel Albedo",      &g_UIControls.drawTexture, DrawTexture::TEXTURE_VOXEL_ALBEDO);
            ImGui::RadioButton("Voxel Normal",      &g_UIControls.drawTexture, DrawTexture::TEXTURE_VOXEL_NORMAL);
            ImGui::RadioButton("Gbuffer Depth",     &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_DEPTH);
            ImGui::RadioButton("Gbuffer Albedo",    &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_ALBEDO);
            ImGui::RadioButton("Gbuffer Normal",    &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_NORMAL);
            ImGui::RadioButton("Gbuffer Metallic",  &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_METALLIC);
            ImGui::RadioButton("Gbuffer Roughness", &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_ROUGHNESS);
            ImGui::RadioButton("Gbuffer Shadow",    &g_UIControls.drawTexture, DrawTexture::TEXTURE_GBUFFER_SHADOW);

            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::BeginMenu("Light"))
        {
            g_scene.lightDirty |= ImGui::SliderFloat("position x", &g_scene.light.position.x, -10.0f, 10.0f);
            g_scene.lightDirty |= ImGui::SliderFloat("position y", &g_scene.light.position.y,  20.0f, 30.0f);
            g_scene.lightDirty |= ImGui::SliderFloat("position z", &g_scene.light.position.z, -10.0f, 10.0f);
            g_scene.dirty |= g_scene.lightDirty;
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::BeginMenu("Log"))
        {
            ImGui::Text("FPS            : %.2f FPS", ImGui::GetIO().Framerate);
            ImGui::Text("Total meshes   : %.d", g_UIControls.totalMeshes);
            ImGui::Text("Total Materials: %.d", g_UIControls.totalMaterials);
            ImGui::Text("Object occluded: %.d", g_UIControls.objectOccluded);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        ImGui::EndMenuBar();
    }

    ImGui::End();

    // Rendering
    ImGui::Render();
}

UIControlls g_UIControls;

} // namespace vct
