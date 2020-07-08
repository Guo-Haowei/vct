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
    void loadObj(const char* path, Scene& scene, Matrix4 transform, Material* customMaterial = nullptr);
private:
    Mesh* processMesh(const aiMesh* aimesh);
    Material* processMaterial(const aiMaterial* aimaterial);
private:
    std::string m_currentPath;
};

} // namespace vct
