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

Mesh* SceneLoader::loadMeshFromObj(const char* path)
{
    std::cout << "[Log] loading model from [" << path << "]" << std::endl;
    ::Assimp::Importer importer;
    const aiScene* aiscene = importer.ReadFile(path, 0
        // aiProcess_Triangulate
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

    assert(aiscene->mMeshes);

    // only process the first mesh for now
    return processMesh(aiscene->mMeshes[0]);
}


Mesh* SceneLoader::processMesh(aiMesh* aimesh)
{
    string name = aimesh->mName.C_Str();
    Mesh* mesh = new Mesh;
    mesh->positions.reserve(aimesh->mNumVertices);
    mesh->normals.reserve(aimesh->mNumVertices);
    bool hasUv = aimesh->mTextureCoords[0];
    if (hasUv)
        mesh->uvs.reserve(aimesh->mNumVertices);

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
    }

    mesh->faces.reserve(aimesh->mNumFaces);
    for (uint32_t i = 0; i < aimesh->mNumFaces; ++i)
    {
        aiFace& face = aimesh->mFaces[i];
        mesh->faces.push_back(Vector3u(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
    }
    // mesh.materialIndex = aimesh->mMaterialIndex;

    return mesh;
}

} // namespace vct
