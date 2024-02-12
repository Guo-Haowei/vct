#include "scene_importer.h"

#include "asset_loader.h"

namespace vct {

SceneImporter::SceneImporter(Scene& scene, const std::string& file_path, const char* loader_name)
    : m_scene(scene), m_file_path(file_path), m_loader_name(loader_name) {

    std::filesystem::path system_path{ file_path };
    m_scene_name = system_path.filename().string();
    m_search_path = system_path.remove_filename().string();
}

auto SceneImporter::import() -> std::expected<void, std::string> {
    if (!import_impl()) {
        return std::unexpected(std::format("[{}] {}.", m_loader_name, m_error));
    }

    // process images
    LOG_VERBOSE("[{}] loading {} images...", m_loader_name, m_scene.get_count<MaterialComponent>());
    for (const MaterialComponent& material : m_scene.get_component_array<MaterialComponent>()) {
        for (int i = 0; i < array_length(material.textures); ++i) {
            const std::string& image_path = material.textures[i].name;
            if (!image_path.empty()) {
                asset_loader::load_image_sync(image_path);
            }
        }
    }

    LOG_VERBOSE("[{}] generating bounding boxes", m_loader_name);
    // process meshes
    for (MeshComponent& mesh : m_scene.get_component_array<MeshComponent>()) {
        mesh.create_render_data();
    }

    // update bounding box
    m_scene.update(0.0f);
    m_scene.m_bound.make_invalid();

    const uint32_t numObjects = (uint32_t)m_scene.get_count<ObjectComponent>();
    for (uint32_t i = 0; i < numObjects; ++i) {
        const ObjectComponent& obj = m_scene.get_component_array<ObjectComponent>()[i];
        ecs::Entity entity = m_scene.get_entity<ObjectComponent>(i);
        DEV_ASSERT(m_scene.contains<TransformComponent>(entity));
        const TransformComponent& transform = *m_scene.get_component<TransformComponent>(entity);
        DEV_ASSERT(m_scene.contains<MeshComponent>(obj.mesh_id));
        const MeshComponent& mesh = *m_scene.get_component<MeshComponent>(obj.mesh_id);

        mat4 M = transform.get_world_matrix();
        AABB aabb = mesh.local_bound;
        aabb.apply_matrix(M);
        m_scene.m_bound.union_box(aabb);
    }

    return std::expected<void, std::string>();
}

}  // namespace vct
