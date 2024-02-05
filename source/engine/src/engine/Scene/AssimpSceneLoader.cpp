#include "AssimpSceneLoader.h"

#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <filesystem>

#include "Core/CommonDvars.h"

using std::string;
using std::vector;
namespace fs = std::filesystem;

void SceneLoader::LoadGLTF(std::string_view path, bool flipUVs) {
    Assimp::Importer importer;

    unsigned int flag = aiProcess_CalcTangentSpace | aiProcess_Triangulate;
    flag |= flipUVs ? aiProcess_FlipUVs : 0;

    const aiScene* aiscene = importer.ReadFile(std::string(path), flag);

    // check for errors
    if (!aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiscene->mRootNode)  // if is Not Zero
    {
        LOG_ERROR("[assimp] failed to load scene '{}'\n\tdetails: {}", path, importer.GetErrorString());
    }

    const uint32_t numMeshes = aiscene->mNumMeshes;
    const uint32_t numMaterials = aiscene->mNumMaterials;

    LOG_VERBOSE("scene '{}' has {} meshes, {} materials", path, numMeshes, numMaterials);

    fs::path fullpath = fs::path(path);
    mCurrentPath = fullpath.parent_path().string() + "/";

    for (uint32_t i = 0; i < numMaterials; ++i) {
        ProcessMaterial(*aiscene->mMaterials[i]);
    }

    for (uint32_t i = 0; i < numMeshes; ++i) {
        ProcessMesh(*aiscene->mMeshes[i]);
    }

    ecs::Entity root = ProcessNode(aiscene->mRootNode, ecs::Entity::INVALID);
    mScene.GetComponent<TagComponent>(root)->SetTag(fullpath.filename().string());

    mScene.mRoot = root;
    // scene bounding box
    mScene.Update(0.0f);
    mScene.bound.make_invalid();

    // @TODO: refactor
    const uint32_t numObjects = (uint32_t)mScene.GetCount<ObjectComponent>();
    for (uint32_t i = 0; i < numObjects; ++i) {
        const ObjectComponent& obj = mScene.GetComponentArray<ObjectComponent>()[i];
        ecs::Entity entity = mScene.GetEntity<ObjectComponent>(i);
        DEV_ASSERT(mScene.Contains<TransformComponent>(entity));
        const TransformComponent& transform = *mScene.GetComponent<TransformComponent>(entity);
        DEV_ASSERT(mScene.Contains<MeshComponent>(obj.meshID));
        const MeshComponent& mesh = *mScene.GetComponent<MeshComponent>(obj.meshID);

        mat4 M = transform.GetWorldMatrix();
        AABB aabb = mesh.mLocalBound;
        aabb.apply_matrix(M);
        mScene.bound.union_box(aabb);
    }
}

void SceneLoader::ProcessMaterial(aiMaterial& material) {
    auto materialID = mScene.Entity_CreateMaterial(std::string("Material::") + material.GetName().C_Str());
    MaterialComponent* materialComponent = mScene.GetComponent<MaterialComponent>(materialID);
    DEV_ASSERT(materialComponent);

    auto getMaterialPath = [&](aiTextureType type, unsigned int index) -> std::string {
        aiString path;
        if (material.GetTexture(type, index, &path) == AI_SUCCESS) {
            return mCurrentPath + path.C_Str();
        }
        return "";
    };

    std::string path = getMaterialPath(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE);
    if (path.empty()) {
        path = getMaterialPath(aiTextureType_DIFFUSE, 0);
    }
    materialComponent->mTextures[MaterialComponent::Base].name = path;

    path = getMaterialPath(aiTextureType_NORMALS, 0);
    if (path.empty()) {
        path = getMaterialPath(aiTextureType_HEIGHT, 0);
    }
    materialComponent->mTextures[MaterialComponent::Normal].name = path;

    materialComponent->mTextures[MaterialComponent::MetallicRoughness].name =
        getMaterialPath(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE);

    mMaterials.emplace_back(materialID);
}

void SceneLoader::ProcessMesh(const aiMesh& mesh) {
    DEV_ASSERT(mesh.mNumVertices);
    const std::string meshName(mesh.mName.C_Str());
    const bool hasUV = mesh.mTextureCoords[0];
    if (!hasUV) {
        LOG_WARN("mesh {} does not have texture coordinates", meshName);
    }

    ecs::Entity meshID = mScene.Entity_CreateMesh("Mesh::" + meshName);
    MeshComponent& meshComponent = *mScene.GetComponent<MeshComponent>(meshID);

    for (uint32_t i = 0; i < mesh.mNumVertices; ++i) {
        auto& position = mesh.mVertices[i];
        meshComponent.mPositions.emplace_back(vec3(position.x, position.y, position.z));
        auto& normal = mesh.mNormals[i];
        meshComponent.mNormals.emplace_back(vec3(normal.x, normal.y, normal.z));
        auto& tangent = mesh.mTangents[i];
        meshComponent.mTangents.emplace_back(vec3(tangent.x, tangent.y, tangent.z));
        auto& bitangent = mesh.mBitangents[i];
        meshComponent.mBitangents.emplace_back(vec3(bitangent.x, bitangent.y, bitangent.z));

        if (hasUV) {
            auto& uv = mesh.mTextureCoords[0][i];
            meshComponent.mTexcoords_0.emplace_back(vec2(uv.x, uv.y));
        } else {
            meshComponent.mTexcoords_0.emplace_back(vec2(0));
        }
    }

    for (uint32_t i = 0; i < mesh.mNumFaces; ++i) {
        aiFace& face = mesh.mFaces[i];
        meshComponent.mIndices.emplace_back(face.mIndices[0]);
        meshComponent.mIndices.emplace_back(face.mIndices[1]);
        meshComponent.mIndices.emplace_back(face.mIndices[2]);
    }

    DEV_ASSERT(mMaterials.size());
    MeshComponent::MeshSubset subset;
    subset.indexCount = (uint32_t)meshComponent.mIndices.size();
    subset.indexOffset = 0;
    subset.materialID = mMaterials.at(mesh.mMaterialIndex);
    meshComponent.mSubsets.emplace_back(subset);

    meshComponent.CreateBounds();

    mMeshes.push_back(meshID);
}

ecs::Entity SceneLoader::ProcessNode(const aiNode* node, ecs::Entity parent) {
    const auto key = std::string(node->mName.C_Str());

    ecs::Entity entity;

    if (node->mNumMeshes == 1) {  // geometry node
        entity = mScene.Entity_CreateObject("Geometry::" + key);

        ObjectComponent& objComponent = *mScene.GetComponent<ObjectComponent>(entity);
        objComponent.meshID = mMeshes[node->mMeshes[0]];
    } else {  // else make it a transform/bone node
        entity = mScene.Entity_CreateTransform("Node::" + key);

        for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
            ecs::Entity child = mScene.Entity_CreateObject("");
            auto tagComponent = mScene.GetComponent<TagComponent>(child);
            tagComponent->SetTag("SubGeometry_" + std::to_string(child.GetID()));
            ObjectComponent& objComponent = *mScene.GetComponent<ObjectComponent>(child);
            objComponent.meshID = mMeshes[node->mMeshes[i]];
            mScene.Component_Attach(child, entity);
        }
    }

    DEV_ASSERT(mScene.Contains<TransformComponent>(entity));

    const aiMatrix4x4& local = node->mTransformation;                       // row major matrix
    mat4 localTransformColumnMajor(local.a1, local.b1, local.c1, local.d1,  // x0 y0 z0 w0
                                   local.a2, local.b2, local.c2, local.d2,  // x1 y1 z1 w1
                                   local.a3, local.b3, local.c3, local.d3,  // x2 y2 z2 w2
                                   local.a4, local.b4, local.c4, local.d4   // x3 y3 z3 w3
    );
    TransformComponent& transform = *mScene.GetComponent<TransformComponent>(entity);
    transform.MatrixTransform(localTransformColumnMajor);

    if (parent.IsValid()) {
        mScene.Component_Attach(entity, parent);
    }

    // process children
    for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
        ProcessNode(node->mChildren[childIndex], entity);
    }

    return entity;
}
