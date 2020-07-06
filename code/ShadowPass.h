#pragma once
#include "common.h"
#include "GL/ShaderProgram.h"
#include "GL/VertexArray.h"
#include <memory>

class ShadowPass
{
public:
    void initialize();
    void render();
    void finalize();
private:
    GLuint m_frameBuffer;
    std::unique_ptr<ShaderProgram> m_shadowShader;
};

