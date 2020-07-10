#pragma once
#include "Scene.h"

struct aiMesh;
struct aiNode;
struct aiMaterial;
struct aiScene;
struct aiAnimation;

namespace vct {

class SceneLoader
{
public:
    void loadGltf(const char* path, Scene& scene, const Matrix4& transform, bool flipUVs = true);
    void loadObj(const char* path, Scene& scene, const Matrix4& transform, Material* customMaterial = nullptr);
private:
    Mesh* processMesh(const aiMesh* aimesh);
    Material* processMaterial(const aiMaterial* aimaterial);
private:
    std::string m_currentPath;
};

} // namespace vct
