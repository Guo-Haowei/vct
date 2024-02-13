#pragma once
#include "scene/scene.h"

namespace vct {

enum ImporterName {
    IMPORTER_TINYGLTF,
    IMPORTER_ASSIMP,
    IMPORTER_MAX,
};

class SceneImporter {
public:
    SceneImporter(Scene& scene, const std::string& file_path, const char* loader_name);

    auto import() -> std::expected<void, std::string>;

    const std::string& get_error() const { return m_error; }

protected:
    virtual bool import_impl() = 0;

    Scene& m_scene;
    std::string m_scene_name;
    std::string m_loader_name;
    std::string m_file_path;
    std::string m_search_path;
    std::string m_error;
};

}  // namespace vct
