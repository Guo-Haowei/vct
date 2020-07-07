#pragma once
#include "MainRenderer.h"
#include "scene/Scene.h"
#include "application/Globals.h"
#include <unordered_map>

namespace vct {

static std::unordered_map<const Mesh*, PerDrawData> g_meshLUT;

void MainRenderer::createGpuResources()
{
    // create shader
    m_basicProgram.createFromFiles(
        DATA_DIR "shaders/basic.vert",
        DATA_DIR "shaders/basic.frag");

    m_boxWireframeProgram.createFromFiles(
        DATA_DIR "shaders/box_wireframe.vert",
        DATA_DIR "shaders/box_wireframe.frag");

    // box wireframe
    {
        std::vector<Vector3> points;
        std::vector<unsigned int> indices;
        three::geometry::boxWireframe(points, indices);

        glGenVertexArrays(1, &m_boxWireframe.vao);
        glGenBuffers(2, &m_boxWireframe.ebo);
        glBindVertexArray(m_boxWireframe.vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_boxWireframe.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_boxWireframe.vbo1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * points.size(), points.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
        glEnableVertexAttribArray(0);

        m_boxWireframe.count = static_cast<unsigned int>(indices.size());
    }

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

void MainRenderer::renderBoundingBox(const Matrix4& PV)
{
    /// TODO: refactor
    static const GLint pvLocation = m_boxWireframeProgram.getUniformLocation("u_PV");
    static const GLint centerLocation = m_boxWireframeProgram.getUniformLocation("u_center");
    static const GLint sizeLocation = m_boxWireframeProgram.getUniformLocation("u_size");
    static const GLint colorLocation = m_boxWireframeProgram.getUniformLocation("u_color");

    // draw box
    if (g_UIControls.showObjectBoundingBox || g_UIControls.showWorldBoundingBox)
    {
        m_boxWireframeProgram.use();
        m_boxWireframeProgram.setUniform(pvLocation, PV);
        glBindVertexArray(m_boxWireframe.vao);
    }

    if (g_UIControls.showObjectBoundingBox)
    {
        m_boxWireframeProgram.setUniform(colorLocation, Vector3::UnitY);
        for (const GeometryNode& node : g_scene.geometryNodes)
        {
            for (const Geometry& geom : node.geometries)
            {
                m_boxWireframeProgram.setUniform(centerLocation, geom.boundingBox.getCenter());
                m_boxWireframeProgram.setUniform(sizeLocation, geom.boundingBox.getSize());
                glDrawElements(GL_LINES, m_boxWireframe.count, GL_UNSIGNED_INT, 0);
            }
        }
    }

    if (g_UIControls.showWorldBoundingBox)
    {
        m_boxWireframeProgram.setUniform(colorLocation, Vector3::UnitZ);
        m_boxWireframeProgram.setUniform(centerLocation, g_scene.boundingBox.getCenter());
        m_boxWireframeProgram.setUniform(sizeLocation, g_scene.boundingBox.getSize());
        glDrawElements(GL_LINES, m_boxWireframe.count, GL_UNSIGNED_INT, 0);
    }
}

void MainRenderer::render()
{
    auto extent = m_pWindow->getFrameExtent();
    glViewport(0, 0, extent.witdh, extent.height);
    // glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_basicProgram.use();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    /// TODO: uniform buffer
    static const GLint PVLocation = m_basicProgram.getUniformLocation("u_PV");
    static const GLint MLocation = m_basicProgram.getUniformLocation("u_M");

    float aspect = (float)extent.witdh / (float)extent.height;
    g_scene.camera.aspect = aspect;

    Matrix4 PV = g_scene.camera.perspective() * g_scene.camera.view();

    m_basicProgram.setUniform(PVLocation, PV);

    for (const GeometryNode& node : g_scene.geometryNodes)
    {
        m_basicProgram.setUniform(MLocation, node.transform);
        for (const Geometry& geom : node.geometries)
        {
            const PerDrawData& drawData = g_meshLUT.find(geom.pMesh)->second;
            glBindVertexArray(drawData.vao);
            glDrawElements(GL_TRIANGLES, drawData.count, GL_UNSIGNED_INT, 0);
        }
    }

    renderBoundingBox(PV);
}

void MainRenderer::destroyGpuResources()
{
    m_basicProgram.destroy();
    m_boxWireframeProgram.destroy();
}

} // namespace vct
