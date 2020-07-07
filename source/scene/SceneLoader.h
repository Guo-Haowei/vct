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
    static void loadObj(const char* path, Scene& scene, Matrix4 transform = Matrix4::Identity);
    static Mesh* processMesh(const aiMesh* aimesh);
};

} // namespace vct
