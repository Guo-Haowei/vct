#include "SceneLoader.h"
#include "base/Exception.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <assert.h>
using std::string;
using std::vector;

namespace vct {

void SceneLoader::loadObj(const char* path, Scene& scene, Matrix4 transform, Material* customMaterial)
{
    std::cout << "[Log] loading model from [" << path << "]" << std::endl;
    ::Assimp::Importer importer;
    const aiScene* aiscene = importer.ReadFile(path,
        aiProcess_Triangulate
        // aiProcess_JoinIdenticalVertices |
        // aiProcess_FlipUVs |
        // aiProcess_GenSmoothNormals |
        // aiProcess_PopulateArmatureData
    );

    // check for errors
    if(!aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiscene->mRootNode) // if is Not Zero
    {
        std::cout << "[ERROR] assimp failed to load.";
        std::cout << importer.GetErrorString() << std::endl;
        string error("assimp: failed to load scene [");
        error.append(path).append("]\n\t").append(importer.GetErrorString());
        THROW_EXCEPTION(error);
    }

    // set base path
    m_currentPath = path;
    auto found = m_currentPath.find_last_of("/\\");
    m_currentPath = m_currentPath.substr(0, found + 1);

    size_t materialOffset = scene.materials.size();

    if (customMaterial)
    {
        scene.materials.push_back(std::move(std::unique_ptr<Material>(customMaterial)));
    }
    else
    {
        for (uint32_t i = 0; i < aiscene->mNumMaterials; ++i)
        {
            const aiMaterial* aimat = aiscene->mMaterials[i];
            Material* mat = processMaterial(aimat);
            scene.materials.push_back(std::move(std::unique_ptr<Material>(mat)));
        }
    }

    GeometryNode node;
    node.transform = transform;

    for (uint32_t i = 0; i < aiscene->mNumMeshes; ++i)
    {
        const aiMesh* aimesh = aiscene->mMeshes[i];
        Mesh* mesh = processMesh(aimesh);
        size_t index = materialOffset + mesh->materialIndex;
        Material* mat = customMaterial ? customMaterial : scene.materials.at(index).get();
        Box3 box;
        box.expandPoints(mesh->positions.size(), mesh->positions.data());
        box.applyMatrix(transform);
        node.geometries.push_back({ mesh, mat, box });
        scene.meshes.push_back(std::move(std::unique_ptr<Mesh>(mesh)));

        scene.boundingBox.mergeBox(box);
    }

    scene.geometryNodes.push_back(node);
}

Material* SceneLoader::processMaterial(const aiMaterial* aimaterial)
{
    Material* mat = new Material;

    aiString albedoPath;
    aiColor3D color;
    if (aimaterial->GetTexture(aiTextureType_AMBIENT, 0, &albedoPath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
    {
        mat->albedoMapPath = m_currentPath;
        mat->albedoMapPath.append(albedoPath.C_Str());
        mat->hasAlbedoMap = true;
    }
    else if (aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color))
    {
        mat->albedoColor = Vector3(color.r, color.g, color.b);
        mat->hasAlbedoMap = false;
    }

    return mat;
}

Mesh* SceneLoader::processMesh(const aiMesh* aimesh)
{
    string name = aimesh->mName.C_Str();
    Mesh* mesh = new Mesh;
    mesh->positions.reserve(aimesh->mNumVertices);
    mesh->normals.reserve(aimesh->mNumVertices);
    mesh->uvs.reserve(aimesh->mNumVertices);
    bool hasUv = aimesh->mTextureCoords[0];

    for (uint32_t i = 0; i < aimesh->mNumVertices; ++i)
    {
        auto& position = aimesh->mVertices[i];
        mesh->positions.push_back(Vector3(position.x, position.y, position.z));
        auto& normal = aimesh->mNormals[i];
        mesh->normals.push_back(Vector3(normal.x, normal.y, normal.z));
        if (hasUv)
        {
            auto& uv = aimesh->mTextureCoords[0][i];
            mesh->uvs.push_back(Vector2(uv.x, uv.y));
        }
        else
            mesh->uvs.push_back(Vector2::Zero);
    }

    mesh->faces.reserve(aimesh->mNumFaces);
    for (uint32_t i = 0; i < aimesh->mNumFaces; ++i)
    {
        aiFace& face = aimesh->mFaces[i];
        mesh->faces.push_back(Vector3u(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
    }

    mesh->materialIndex = aimesh->mMaterialIndex;

    return mesh;
}

} // namespace vct
