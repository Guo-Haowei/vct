#include "gl_utils.h"

#include <filesystem>
#include <set>

#include "cbuffer.glsl.h"
#include "core/dynamic_variable/common_dvars.h"

static MeshData g_quad;

void R_CreateQuad() {
    // clang-format off
    float points[] = { -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, };
    // clang-format on
    glGenVertexArrays(1, &g_quad.vao);
    glGenBuffers(1, g_quad.vbos);
    glBindVertexArray(g_quad.vao);

    glBindBuffer(GL_ARRAY_BUFFER, g_quad.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
}

void R_DrawQuad() {
    DEV_ASSERT(g_quad.vao);
    glBindVertexArray(g_quad.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FillMaterialCB(const MaterialData *mat, MaterialConstantBuffer &cb) {
    cb.AlbedoColor = mat->albedoColor;
    cb.Metallic = mat->metallic;
    cb.Roughness = mat->roughness;
    cb.HasAlbedoMap = mat->albedoMap.GetHandle() != 0;
    cb.HasNormalMap = mat->materialMap.GetHandle() != 0;
    cb.HasPbrMap = mat->materialMap.GetHandle() != 0;
    cb.TextureMapIdx = mat->textureMapIdx;
    cb.ReflectPower = mat->reflectPower;
}

namespace gl {

//------------------------------------------------------------------------------
// Constant Buffer
//------------------------------------------------------------------------------
GLuint CreateAndBindConstantBuffer(int slot, size_t size_in_byte) {
    GLuint handle = 0;
    glGenBuffers(1, &handle);
    glBindBuffer(GL_UNIFORM_BUFFER, handle);
    glBufferData(GL_UNIFORM_BUFFER, size_in_byte, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, slot, handle);
    LOG_VERBOSE("[opengl] created buffer of size {} (slot {})", size_in_byte, slot);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return handle;
}

void UpdateConstantBuffer(GLuint handle, const void *ptr, size_t size_in_byte) {
    // glMapBuffer( m_handle, 0 );
    glBindBuffer(GL_UNIFORM_BUFFER, handle);
    glBufferData(GL_UNIFORM_BUFFER, size_in_byte, ptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

}  // namespace gl