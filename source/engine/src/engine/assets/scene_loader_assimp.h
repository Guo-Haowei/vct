#pragma once
#include "scene/scene.h"

struct aiMesh;
struct aiNode;
struct aiMaterial;
struct aiScene;
struct aiAnimation;

namespace vct {

class SceneLoader {
public:
    SceneLoader(Scene& scene) : mScene(scene) {}

    void LoadGLTF(std::string_view path, bool flipUVs = true);

private:
    void ProcessMaterial(aiMaterial& material);
    void ProcessMesh(const aiMesh& mesh);
    ecs::Entity ProcessNode(const aiNode* node, ecs::Entity parent);

private:
    std::string mCurrentPath;
    Scene& mScene;
    std::vector<ecs::Entity> mMaterials;
    std::vector<ecs::Entity> mMeshes;
};

}  // namespace vct
