#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../math/GeoMath.h"

struct aiMesh;
struct aiNode;
struct aiMaterial;

struct SceneMaterial
{
    std::string name;
    std::string diffusePath;
    std::string normalPath;
};

struct SceneMesh
{
    std::string                 name;
    std::vector<vec3>  positions;
    std::vector<vec3>  normals;
    // TODO: change to unsigned short if possible
    std::vector<unsigned int>   indices;
    // std::vector<vec3>  tagents;
    // std::vector<vec3>  bitTagents;
    // std::vector<vec2>  uvs;
    // int materialIndex = -1;
    Box3D aabb;
};

// struct SceneNode
// {
//     using Ptr = std::unique_ptr<SceneNode>;
//     SceneMesh* mesh;

//     std::vector<Ptr> child;
//     // material
// };

struct Scene
{
    std::string name;
    std::vector<std::unique_ptr<SceneMesh>> meshes;
    std::vector<std::unique_ptr<SceneMaterial>> materials;
    Box3D aabb;
};

class AssimpLoader
{
public:
    Scene* parse(const char* root, const char* file);

private:
    void processMesh(aiMesh* mesh);

    // void processMaterial(aiMaterial* material);

    void processNode(aiNode* node);

    Scene* m_scene;

    std::string m_dir;
};
