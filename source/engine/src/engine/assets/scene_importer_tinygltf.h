#include "scene_importer.h"

namespace tinygltf {
class Model;
struct Animation;
struct Mesh;
}  // namespace tinygltf

namespace vct {

class SceneImporterTinyGLTF : public SceneImporter {
public:
    SceneImporterTinyGLTF(Scene& scene, const std::string& file_path)
        : SceneImporter(scene, file_path, "SceneLoaderTinyGLTF") {}

    bool import_impl() override;

protected:
    void process_node(int node_index, ecs::Entity parent);
    void process_mesh(const tinygltf::Mesh& gltf_mesh, int id);
    void process_animation(const tinygltf::Animation& gltf_anim, int id);

    std::unordered_map<int, ecs::Entity> m_entity_map;
    std::shared_ptr<tinygltf::Model> m_model;
};

}  // namespace vct
