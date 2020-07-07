#pragma once
#include "MainRenderer.h"
#include "scene/Scene.h"
#include <unordered_map>

namespace vct {

struct PerDrawData
{
    GLuint vao;
    // TODO: refactor
    GLuint ebo;
    GLuint vbo1;
    GLuint vbo2;
    unsigned int count;
};

static std::unordered_map<const Mesh*, PerDrawData> g_meshLUT;

void MainRenderer::createGpuResources()
{
    // create shader
    m_basic.createFromFiles(
        DATA_DIR "shaders/basic.vert",
        DATA_DIR "shaders/basic.frag");

    // load scene
    for (const std::unique_ptr<Mesh>& mesh : g_scene.meshes)
    {
        PerDrawData data;
        glGenVertexArrays(1, &data.vao);
        glGenBuffers(3, &data.ebo);
        glBindVertexArray(data.vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Vector3u) * mesh->faces.size(), mesh->faces.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, data.vbo1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * mesh->positions.size(), mesh->positions.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, data.vbo2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * mesh->normals.size(), mesh->normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)0);
        glEnableVertexAttribArray(1);

        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        data.count = 3 * static_cast<unsigned int>(mesh->faces.size());

        g_meshLUT.insert({ mesh.get(), data });
    }

}

void MainRenderer::render()
{
    auto extent = m_pWindow->getFrameExtent();
    glViewport(0, 0, extent.witdh, extent.height);
    // glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_basic.use();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    /// TODO: uniform buffer
    static const GLint PVLocation = m_basic.getUniformLocation("u_PV");
    static const GLint MLocation = m_basic.getUniformLocation("u_M");

    float aspect = (float)extent.witdh / (float)extent.height;
    g_scene.camera.aspect = aspect;

    Matrix4 PV = g_scene.camera.perspective() * g_scene.camera.view();

    m_basic.setUniform(PVLocation, PV);

    for (const GeometryNode& node : g_scene.geometryNodes)
    {
        m_basic.setUniform(MLocation, node.transform);
        for (const Geometry& geom : node.geometries)
        {
            const PerDrawData& drawData = g_meshLUT.find(geom.pMesh)->second;
            glBindVertexArray(drawData.vao);
            glDrawElements(GL_TRIANGLES, drawData.count, GL_UNSIGNED_INT, 0);
        }
    }
}

void MainRenderer::destroyGpuResources()
{
    m_basic.destroy();
}

} // namespace vct
