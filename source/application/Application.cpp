#include "Application.h"
#include "base/Exception.h"
#include "imgui/imgui.h"
#include "scene/CreateScene.h"
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
    constexpr float CAMERA_SPEED = 0.1f;

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
    static bool show_demo_window = true;
    static bool show_another_window = false;

    ImGui::NewFrame();

    // if (show_demo_window)
    //     ImGui::ShowDemoWindow(&show_demo_window);

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Debug");

    ImGui::Checkbox("Visualize voxels", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

    if (ImGui::Button("Button"))
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    // 3. Show another simple window.
    // if (show_another_window)
    // {
    //     ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    //     ImGui::Text("Hello from another window!");
    //     if (ImGui::Button("Close Me"))
    //         show_another_window = false;
    //     ImGui::End();
    // }

    // Rendering
    ImGui::Render();
}

} // namespace vct
