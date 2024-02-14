#include "render_data.h"

#include "core/base/rid_owner.h"
#include "core/math/frustum.h"
#include "gl_utils.h"
#include "r_cbuffers.h"
#include "scene/scene.h"

extern vct::RIDAllocator<MeshData> g_meshes;
extern vct::RIDAllocator<MaterialData> g_materials;

namespace vct {

void RenderData::clear() {
    scene = nullptr;
    shadow_pass.clear();
    main_pass.clear();
}

void RenderData::update(const Scene* p_scene) {
    clear();
    scene = p_scene;

    fill(
        p_scene,
        g_perFrameCache.cache.c_light_matricies[0],
        [](const ObjectComponent& object) {
            return !(object.flags & ObjectComponent::CAST_SHADOW) || !(object.flags & ObjectComponent::RENDERABLE);
        },
        shadow_pass);
    fill(
        p_scene,
        g_perFrameCache.cache.c_projection_view_matrix,
        [](const ObjectComponent& object) {
            return !(object.flags & ObjectComponent::RENDERABLE);
        },
        main_pass);
}

void RenderData::fill(const Scene* p_scene, const mat4& projection_view_matrix, FilterObjectFunc filter, Pass& pass) {
    scene = p_scene;
    pass.projection_view_matrix = projection_view_matrix;
    Frustum frustum{ projection_view_matrix };
    uint32_t num_objects = (uint32_t)scene->get_count<ObjectComponent>();
    for (uint32_t i = 0; i < num_objects; ++i) {
        const ObjectComponent& obj = scene->get_component_array<ObjectComponent>()[i];
        if (filter(obj)) {
            continue;
        }

        ecs::Entity entity = scene->get_entity<ObjectComponent>(i);
        DEV_ASSERT(scene->contains<TransformComponent>(entity));
        const TransformComponent& transform = *scene->get_component<TransformComponent>(entity);
        DEV_ASSERT(scene->contains<MeshComponent>(obj.mesh_id));
        const MeshComponent& mesh = *scene->get_component<MeshComponent>(obj.mesh_id);

        const mat4& world_matrix = transform.get_world_matrix();
        AABB aabb = mesh.local_bound;
        aabb.apply_matrix(world_matrix);
        if (!frustum.intersects(aabb)) {
            continue;
        }

        Mesh draw;
        draw.armature_id = mesh.armature_id;
        draw.world_matrix = world_matrix;
        const MeshData* mesh_data = g_meshes.get_or_null(mesh.gpu_resource);
        DEV_ASSERT(mesh_data);
        draw.mesh_data = mesh_data;

        for (const auto& subset : mesh.subsets) {
            aabb = subset.local_bound;
            aabb.apply_matrix(world_matrix);
            if (!frustum.intersects(aabb)) {
                continue;
            }

            const MaterialComponent& material = *scene->get_component<MaterialComponent>(subset.material_id);
            const MaterialData* mat_data = g_materials.get_or_null(material.gpu_resource);
            DEV_ASSERT(mat_data);

            SubMesh sub_mesh;
            sub_mesh.index_count = subset.index_count;
            sub_mesh.index_offset = subset.index_offset;
            sub_mesh.material_data = mat_data;
            draw.subsets.emplace_back(std::move(sub_mesh));
        }

        pass.draws.emplace_back(std::move(draw));
    }
}

}  // namespace vct
