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
    static Mesh* loadMeshFromObj(const char* path);
    static Mesh* processMesh(aiMesh* aimesh);
};

} // namespace vct
