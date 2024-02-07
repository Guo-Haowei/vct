#include "r_editor.h"

#include "Core/geometry.h"
#include "Framework/ProgramManager.h"
#include "GpuTexture.h"
#include "gl_utils.h"
#include "r_cbuffers.h"
#include "scene/scene_manager.h"

using namespace vct;

struct VertexPoint3D {
    vec3 position;
    vec3 color;
};

static MeshData g_boxWireFrame;
static MeshData g_gridWireFrame;
static MeshData g_imageBuffer;

struct TextureVertex {
    vec2 pos;
    vec2 uv;
};

static void CreateImageBuffer() {
    MeshData& mesh = g_imageBuffer;
    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, mesh.vbos);
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void*)(sizeof(vec2)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

static void CreateBoxWireFrameData() {
    std::vector<VertexPoint3D> vertices;
    const MeshComponent box = MakeBoxWireFrame();
    vertices.reserve(box.mPositions.size());
    for (const vec3& pos : box.mPositions) {
        vertices.emplace_back(VertexPoint3D{ pos, vec3(1) });
    }

    MeshData& mesh = g_boxWireFrame;

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(2, &mesh.ebo);
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);

    gl::NamedBufferStorage(mesh.vbos[0], vertices);
    gl::NamedBufferStorage(mesh.ebo, box.mIndices);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPoint3D), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPoint3D), (void*)(sizeof(vec3)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    g_boxWireFrame.count = uint32_t(box.mIndices.size());
}

void R_CreateEditorResource() {
    CreateBoxWireFrameData();
    CreateImageBuffer();
}

void R_DestroyEditorResource() {
    glDeleteVertexArrays(1, &g_boxWireFrame.vao);
    glDeleteBuffers(2, &g_boxWireFrame.ebo);
}

static inline void FillTextureIconBuffer(std::vector<TextureVertex>& iconBuffer, const vec2& offset, float aspect) {
    constexpr TextureVertex kVertices[] = {
        { vec2(-1, +1), vec2(0, 0) },  // top-left
        { vec2(-1, -1), vec2(0, 1) },  // bottom-left
        { vec2(+1, -1), vec2(1, 1) },  // bottom-right
        { vec2(+1, +1), vec2(1, 0) },  // top-right
    };

    constexpr uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };
    constexpr float kScale = 0.07f;
    const vec2 scale(kScale, aspect * kScale);

    for (size_t idx = 0; idx < vct::array_length(indices); ++idx) {
        TextureVertex vertex = kVertices[indices[idx]];
        vertex.pos *= scale;
        vertex.pos += offset;
        iconBuffer.emplace_back(vertex);
    }
}

// draw grid, bounding box, ui
void R_DrawEditor() {
    glDisable(GL_DEPTH_TEST);
    const Scene& scene = SceneManager::get_scene();
    auto selected = scene.m_selected;
    if (selected.IsValid()) {
        auto transformComponent = scene.get_component<TransformComponent>(selected);
        DEV_ASSERT(transformComponent);
        auto objComponent = scene.get_component<ObjectComponent>(selected);
        if (objComponent) {
            DEV_ASSERT(objComponent);
            auto meshComponent = scene.get_component<MeshComponent>(objComponent->meshID);
            DEV_ASSERT(meshComponent);
            AABB aabb = meshComponent->mLocalBound;
            aabb.apply_matrix(transformComponent->GetWorldMatrix());

            const mat4 M = glm::translate(mat4(1), aabb.center()) * glm::scale(mat4(1), aabb.size());

            gProgramManager->GetShaderProgram(ProgramType::LINE3D).Bind();
            glBindVertexArray(g_boxWireFrame.vao);
            g_perBatchCache.cache.PVM = g_perFrameCache.cache.PV * M;
            g_perBatchCache.cache.Model = mat4(1);
            g_perBatchCache.Update();
            glDrawElements(GL_LINES, g_boxWireFrame.count, GL_UNSIGNED_INT, 0);
        }
    }

    // AABB aabb(vec3(-1), vec3(1));
    // const mat4 M = glm::translate(mat4(1), aabb.center()) * glm::scale(mat4(1), aabb.size());

    // gProgramManager->GetShaderProgram(ProgramType::LINE3D).Bind();
    // glBindVertexArray(g_boxWireFrame.vao);
    // g_perBatchCache.cache.PVM = g_perFrameCache.cache.PV * M;
    // g_perBatchCache.cache.Model = mat4(1);
    // g_perBatchCache.Update();
    // glDrawElements(GL_LINES, g_boxWireFrame.count, GL_UNSIGNED_INT, 0);
}