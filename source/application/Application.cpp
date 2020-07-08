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

            userInterface();
            m_renderSystem.update();

            m_window.swapBuffers();
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
    constexpr float CAMERA_SPEED = 0.2f;

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
            ImGui::Text("Voxel Texture Size: %d", VOXEL_TEXTURE_SIZE);
            ImGui::Text("Voxel Mip Level: %d", VOXEL_TEXTURE_MIP_LEVEL);
            if (ImGui::Checkbox("Force voxel texture update", &g_UIControls.forceUpdateVoxelTexture))
                g_scene.dirty = true;
            ImGui::Separator();
            ImGui::RadioButton("None", &g_UIControls.renderVoxel, 0);
            ImGui::RadioButton("Albedo Voxel Texture", &g_UIControls.renderVoxel, 1);
            ImGui::RadioButton("Normal Voxel Texture", &g_UIControls.renderVoxel, 2);
            ImGui::SliderInt("Voxel Mipmap Level", &g_UIControls.voxelMipLevel, 0, VOXEL_TEXTURE_MIP_LEVEL - 1);
            ImGui::Separator();
            if (ImGui::Checkbox("Show Object Bounding Box", &g_UIControls.showObjectBoundingBox))
                g_UIControls.showWorldBoundingBox = false;
            if (ImGui::Checkbox("Show World Bounding Box", &g_UIControls.showWorldBoundingBox))
                g_UIControls.showObjectBoundingBox = false;
            ImGui::EndMenu();
        }

        ImGui::Separator();

        ImGui::Text("FPS: %.2f FPS", ImGui::GetIO().Framerate);

        ImGui::EndMenuBar();
    }

    ImGui::End();

    // Rendering
    ImGui::Render();
}

UIControlls g_UIControls;

} // namespace vct
