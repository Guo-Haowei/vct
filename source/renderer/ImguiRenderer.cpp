#include "ImguiRenderer.h"
#include "imgui_impl_opengl3.h"

namespace vct {

void ImguiRenderer::createGpuResources()
{
    // create shader
    m_program.createFromFiles(DATA_DIR "shaders/imgui.vert",
                              DATA_DIR "shaders/imgui.frag");

    ImGui_ImplOpenGL3_CreateDeviceObjects(m_program.getHandle());
}

void ImguiRenderer::render()
{
    // use shader
    m_program.use();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiRenderer::destroyGpuResources()
{
    m_program.destroy();

    ImGui_ImplOpenGL3_DestroyDeviceObjects();
}

} // namespace vct

