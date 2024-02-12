#pragma once
#include "scene_importer.h"

struct aiMesh;
struct aiNode;
struct aiMaterial;
struct aiScene;
struct aiAnimation;

namespace vct {

class SceneImporterAssimp : public SceneImporter {
public:
    SceneImporterAssimp(Scene& scene, const std::string& file_path) : SceneImporter(scene, file_path, "SceneLoaderAssimp") {}

protected:
    bool import_impl() override;

    void process_material(aiMaterial& material);
    void process_mesh(const aiMesh& mesh);

    ecs::Entity process_node(const aiNode* node, ecs::Entity parent);

    std::vector<ecs::Entity> m_materials;
    std::vector<ecs::Entity> m_meshes;
};

}  // namespace vct
