#pragma once
#include "MainRenderer.h"

namespace vct {

// TODO: refactor
unsigned int VBO, VAO;

void MainRenderer::createGpuResources()
{
    // create shader
    m_basic.createFromFiles(
        DATA_DIR "shaders/basic.vert",
        DATA_DIR "shaders/basic.frag");


    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
         0.5f, -0.5f, 0.0f, // right
         0.0f,  0.5f, 0.0f  // top
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

void MainRenderer::render()
{
    auto extent = m_pWindow->getFrameExtent();
    glViewport(0, 0, extent.witdh, extent.height);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_basic.use();
    /// TODO: uniform buffer
    static const GLint PVLocation = m_basic.getUniformLocation("u_PV");
    static const GLint MLocation = m_basic.getUniformLocation("u_M");

    float aspect = (float)extent.witdh / (float)extent.height;

    Matrix4 V = three::lookAt(Vector3::UnitZ, Vector3::Zero, Vector3::UnitY);
    Matrix4 P = three::perspectiveRH_NO(1.0f, aspect, 0.1f, 100.0f);
    Matrix4 PV = P * V;
    static Vector3 translation = Vector3::Zero;
    translation.z -= 0.01f;
    Matrix4 M = three::translate(translation);
    // Matrix4 M = Matrix4::Identity;

    m_basic.setUniform(PVLocation, PV);
    m_basic.setUniform(MLocation, M);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void MainRenderer::destroyGpuResources()
{
    m_basic.destroy();
}

} // namespace vct
