#pragma once
#include "common/Scene.h"

struct aiMesh;
struct aiNode;
struct aiMaterial;
struct aiScene;
struct aiAnimation;

namespace vct {

class SceneLoader {
   public:
    void loadGltf( const char* path, Scene& scene, const mat4& transform, bool flipUVs = true );

   private:
    MeshComponent* processMesh( const aiMesh* aimesh );
    Material* processMaterial( const aiMaterial* aimaterial );

   private:
    std::string m_currentPath;
};

}  // namespace vct
