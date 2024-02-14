#pragma once
#include "gl_utils.h"
#include "scene/scene.h"

namespace vct {

struct RenderData {
    using FilterObjectFunc = bool (*)(const ObjectComponent& object);

    struct SubMesh {
        uint32_t index_count;
        uint32_t index_offset;
        const MaterialData* material_data;
    };

    struct Mesh {
        ecs::Entity armature_id;
        mat4 world_matrix;
        const MeshData* mesh_data;
        std::vector<SubMesh> subsets;
    };

    struct Pass {
        mat4 projection_view_matrix;
        std::vector<Mesh> draws;

        void clear() { draws.clear(); }
    };

    const Scene* scene = nullptr;

    Pass shadow_pass;
    Pass main_pass;

    void update(const Scene* p_scene);

private:
    void clear();

    void fill(const Scene* p_scene, const mat4& projection_view_matrix, FilterObjectFunc filter, Pass& pass);
};

}  // namespace vct