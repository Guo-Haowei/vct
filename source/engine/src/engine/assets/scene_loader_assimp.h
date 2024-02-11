#pragma once
#include "scene_loader.h"

struct aiMesh;
struct aiNode;
struct aiMaterial;
struct aiScene;
struct aiAnimation;

namespace vct {

class SceneLoaderAssimp : public SceneLoader {
public:
    SceneLoaderAssimp(Scene& scene, const std::string& file_path) : SceneLoader(scene, file_path, "SceneLoaderAssimp") {}

protected:
    bool import_impl() override;

    void process_material(aiMaterial& material);
    void process_mesh(const aiMesh& mesh);

    ecs::Entity process_node(const aiNode* node, ecs::Entity parent);

    std::vector<ecs::Entity> m_materials;
    std::vector<ecs::Entity> m_meshes;
};

}  // namespace vct
