#pragma once
#include "SceneGraph/Scene.hpp"

struct aiMesh;
struct aiNode;
struct aiMaterial;
struct aiScene;
struct aiAnimation;

class AssimpLoader {
public:
    void loadGltf( const char* path, Scene& scene, bool flipUVs = true );

private:
    MeshComponent* processMesh( const aiMesh* aimesh );
    Material* processMaterial( const aiMaterial* aimaterial );

private:
    std::string m_currentPath;
};
