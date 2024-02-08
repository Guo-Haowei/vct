#include "scene_loader.h"

namespace tinygltf {
class Model;
}

namespace vct {

class SceneLoaderTinyGLTF : public SceneLoader {
public:
    SceneLoaderTinyGLTF(Scene& scene, const std::string& file_path)
        : SceneLoader(scene, file_path, "SceneLoaderTinyGLTF") {}

    virtual bool import_impl() override;

protected:
    void process_node(int node_index, ecs::Entity parent);

    std::unordered_map<int, ecs::Entity> m_entity_map;
    std::shared_ptr<tinygltf::Model> m_model;
};

}  // namespace vct
