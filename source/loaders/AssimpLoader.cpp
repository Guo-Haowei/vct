#include "AssimpLoader.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <iostream>
#include <assert.h>

Scene* AssimpLoader::parse(const char* root, const char* file)
{
    m_dir = root;
    std::string filePath = m_dir + "/" + std::string(file);
    std::cout << "[Log] Loading scene [" << filePath << "]" << std::endl;
    ::Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        // aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices
    );
    m_scene = new Scene();
    m_scene->name = file;

    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "[ERROR] assimp failed to load.";
        std::cout << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    // m_scene->materials.reserve(scene->mNumMaterials);
    // for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
    // {
    //     aiMaterial* material = scene->mMaterials[i];
    //     processMaterial(scene->mMaterials[i]);
    // }

    m_scene->meshes.reserve(scene->mNumMeshes);
    for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
    {
        processMesh(scene->mMeshes[i]);
    }

    // for (uint32_t i = 0; i < scene->mNumTextures; ++i)
    // {
    //     aiTexture* texture = scene->mTextures[i];
    // }
    // retrieve the directory path of the filepath
    //std::string directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    // processNode(scene->mRootNode, scene);
    std::cout << "[LOG] Scene loaded correctly." << std::endl;
    std::cout << "..............................." << std::endl;

    std::cout << "[LOG] Calculating bounding box\n" << std::endl;
    for (auto& mesh : m_scene->meshes)
    {
        for (auto &v : mesh->positions)
        {
            mesh->aabb.expand(v);
        }

        std::cout << "\tmesh " << mesh->name << " has bbox " << mesh->aabb << std::endl;
        m_scene->aabb.expand(mesh->aabb);
    }
    std::cout << "\tscene " << m_scene->name << " has bbox " << m_scene->aabb << std::endl;

    return m_scene;
}

void AssimpLoader::processMesh(aiMesh* assimpMesh)
{
    SceneMesh* mesh = new SceneMesh();
    // std::cout << "Mesh " << assimpMesh->mName.C_Str() << " has material index " << assimpMesh->mMaterialIndex << std::endl;
    mesh->positions.reserve(assimpMesh->mNumVertices);
    // mesh->materialIndex = assimpMesh->mMaterialIndex;
    for (uint32_t i = 0; i < assimpMesh->mNumVertices; ++i)
    {
        auto& position = assimpMesh->mVertices[i];
        mesh->positions.push_back(vec3(
            position.x,
            position.y,
            position.z
        ));
        auto& normal = assimpMesh->mNormals[i];
        mesh->normals.push_back(vec3(
            normal.x,
            normal.y,
            normal.z
        ));
        // auto& tagent = assimpMesh->mTangents[i];
        // mesh->tagents.push_back(Vector3f(
        //     tagent.x,
        //     tagent.y,
        //     tagent.z
        // ));
        // auto& bitTagents = assimpMesh->mBitangents[i];
        // mesh->bitTagents.push_back(Vector3f(
        //     bitTagents.x,
        //     bitTagents.y,
        //     bitTagents.z
        // ));
        // if (assimpMesh->mTextureCoords[0])
        // {
        //     auto& uv = assimpMesh->mTextureCoords[0][i];
        //     mesh->uvs.push_back(Vector2f(
        //         uv.x,
        //         uv.y
        //     ));
        // }
        // else
        // {
        //     mesh->uvs.push_back(Vector2f(0.0f));
        // }
        // tagent
    }

    mesh->indices.reserve(assimpMesh->mNumFaces * 3);
    for (uint32_t i = 0; i < assimpMesh->mNumFaces; ++i)
    {
        aiFace& face = assimpMesh->mFaces[i];
        assert(face.mNumIndices == 3);
        mesh->indices.push_back(face.mIndices[0]);
        mesh->indices.push_back(face.mIndices[1]);
        mesh->indices.push_back(face.mIndices[2]);
    }

    // auto& min = assimpMesh->mAABB.mMin;
    // auto& max = assimpMesh->mAABB.mMax;

    // bonding box
    vec3 min = mesh->positions.front();
    vec3 max = mesh->positions.front();

    for (auto& point : mesh->positions)
    {
        if (point.x > max.x) max.x = point.x;
        if (point.y > max.y) max.y = point.y;
        if (point.z > max.z) max.z = point.z;
        if (point.x < min.x) min.x = point.x;
        if (point.y < min.y) min.y = point.y;
        if (point.z < min.z) min.z = point.z;
    }

    // mesh->aabb.min = min;
    // mesh->aabb.max = max;

    m_scene->meshes.push_back(std::unique_ptr<SceneMesh>(mesh));
}

#if 0
void AssimpLoader::processMaterial(aiMaterial* material)
{
    SceneMaterial* mat = new SceneMaterial();
    mat->name = material->GetName().C_Str();
    std::cout << "Loading " << mat->name << std::endl;
    aiString path;
    if (material->GetTexture(aiTextureType_AMBIENT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
    {
        std::cout << "Albedo map found: " << path.C_Str() << std::endl;
        mat->diffusePath = m_dir + "/" + std::string(path.C_Str());
    }
    if (material->GetTexture(aiTextureType_DISPLACEMENT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
    {
        std::cout << "Normal map found: " << path.C_Str() << std::endl;
        mat->normalPath = m_dir + "/" + std::string(path.C_Str());
    }

    m_scene->materials.push_back(std::unique_ptr<SceneMaterial>(mat));
}

#endif

void AssimpLoader::processNode(aiNode* node)
{

}
