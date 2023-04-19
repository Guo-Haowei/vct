#include "scene_loader.h"

#include <assert.h>
#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "Core/CommonDvars.h"
#include "Core/Check.h"
#include "Core/DynamicVariable.h"
#include "Core/Log.h"

using std::string;
using std::vector;

void SceneLoader::loadGltf(const char* fullpath, Scene& scene, const mat4& transform, bool flipUVs)
{
    Assimp::Importer importer;

    unsigned int flag = aiProcess_CalcTangentSpace | aiProcess_Triangulate;
    flag |= flipUVs ? aiProcess_FlipUVs : 0;
    const aiScene* aiscene = importer.ReadFile(fullpath, flag);

    // check for errors
    if (!aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiscene->mRootNode)  // if is Not Zero
    {
        LOG_ERROR("[assimp] failed to load scene '{}'\n\tdetails: {}", fullpath, importer.GetErrorString());
    }

    const uint32_t numMeshes = aiscene->mNumMeshes;
    const uint32_t numMaterials = aiscene->mNumMaterials;

    LOG_DEBUG("scene '{}' has {} meshes, {} materials", fullpath, numMeshes, numMaterials);

    // set base path
    m_currentPath = fullpath;
    auto found = m_currentPath.find_last_of("/\\");
    m_currentPath = m_currentPath.substr(0, found + 1);

    size_t materialOffset = scene.materials.size();

    for (uint32_t i = 0; i < numMaterials; ++i)
    {
        const aiMaterial* aimat = aiscene->mMaterials[i];
        Material* mat = processMaterial(aimat);
        scene.materials.emplace_back(std::shared_ptr<Material>(mat));
    }

    GeometryNode node;
    // node.transform = transform;
    node.transform = mat4(1);

    for (uint32_t i = 0; i < numMeshes; ++i)
    {
        const aiMesh* aimesh = aiscene->mMeshes[i];
        std::shared_ptr<MeshComponent> mesh(processMesh(aimesh));
        const size_t index = materialOffset + mesh->materialIdx;

        std::shared_ptr<Material>& mat = scene.materials.at(index);
        AABB box;

        for (vec3& position : mesh->positions)
        {
            position = transform * vec4(position, 1.0);
        }
        for (vec3& normal : mesh->normals)
        {
            normal = mat3(transform) * normal;
        }

        for (const vec3& point : mesh->positions)
        {
            box.Expand(point);
        }

        // slightly enlarge bounding box
        const vec3 offset = vec3(0.01f);
        box.MakeValid();

        // box.ApplyMatrix( transform );
        Geometry geom;
        geom.boundingBox = box;
        geom.material = mat;
        geom.mesh = mesh;

        node.geometries.emplace_back(geom);
        scene.meshes.emplace_back(mesh);
        scene.boundingBox.Union(box);

        // HACK configure floor
        // if ( DVAR_GET_BOOL( r_mirrorFloor ) && mesh->name == "meshes_0-46" )
        // {
        //     mat->reflectPower = 1.0;
        //     mat->albedo       = vec3( 1.0f );
        //     mat->metallic     = 0.0f;
        //     mat->roughness    = 1.0f;

        //     mat->albedoTexture            = "";
        //     mat->metallicRoughnessTexture = "";
        //     mat->normalTexture            = "";
        // }
    }

    scene.geometryNodes.push_back(node);
}

Material* SceneLoader::processMaterial(const aiMaterial* aimaterial)
{
    Material* mat = new Material;

    /// albedo
    {
        aiString path;
        if (aimaterial->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &path) == AI_SUCCESS)
        {
            mat->albedoTexture = m_currentPath;
            mat->albedoTexture.append(path.C_Str());
        }
        else if (aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
        {
            mat->albedoTexture = m_currentPath;
            mat->albedoTexture.append(path.C_Str());
        }
        else
        {
            LOG_WARN("[scene] mesh does not have diffuse texture");
        }
    }

    /// metallic roughness
    {
        aiString path;
        if (aimaterial->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &path) == AI_SUCCESS)
        {
            mat->metallicRoughnessTexture = m_currentPath;
            mat->metallicRoughnessTexture.append(path.C_Str());
        }
    }

    /// normal texture
    {
        aiString path;
        if (aimaterial->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
        {
            mat->normalTexture = m_currentPath;
            mat->normalTexture.append(path.C_Str());
        }
        else if (aimaterial->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS)
        {
            mat->normalTexture = m_currentPath;
            mat->normalTexture.append(path.C_Str());
        }
    }

    return mat;
}

MeshComponent* SceneLoader::processMesh(const aiMesh* aimesh)
{
    check(aimesh->mNumVertices);

    MeshComponent* mesh = new MeshComponent;
    mesh->positions.reserve(aimesh->mNumVertices);
    mesh->normals.reserve(aimesh->mNumVertices);
    mesh->uvs.reserve(aimesh->mNumVertices);
    mesh->name = aimesh->mName.C_Str();

    bool hasUv = aimesh->mTextureCoords[0];

    mesh->flags = MeshComponent::HAS_UV_FLAG | MeshComponent::HAS_NORMAL_FLAG;

    for (uint32_t i = 0; i < aimesh->mNumVertices; ++i)
    {
        auto& position = aimesh->mVertices[i];
        mesh->positions.emplace_back(vec3(position.x, position.y, position.z));
        auto& normal = aimesh->mNormals[i];
        mesh->normals.emplace_back(vec3(normal.x, normal.y, normal.z));

        if (hasUv)
        {
            auto& uv = aimesh->mTextureCoords[0][i];
            mesh->uvs.emplace_back(vec2(uv.x, uv.y));
        }
        else
        {
            mesh->uvs.emplace_back(vec2(0));
        }
    }

    bool hasTangent = aimesh->mTangents != nullptr;
    if (aimesh->mTangents)
    {
        mesh->flags |= MeshComponent::HAS_BITANGENT_FLAG;
        mesh->tangents.reserve(aimesh->mNumVertices);
        mesh->bitangents.reserve(aimesh->mNumVertices);
        for (uint32_t i = 0; i < aimesh->mNumVertices; ++i)
        {
            auto& tangent = aimesh->mTangents[i];
            mesh->tangents.emplace_back(vec3(tangent.x, tangent.y, tangent.z));
            auto& bitangent = aimesh->mBitangents[i];
            mesh->bitangents.emplace_back(vec3(bitangent.x, bitangent.y, bitangent.z));
        }
    }

    mesh->indices.reserve(aimesh->mNumFaces * 3);
    for (uint32_t i = 0; i < aimesh->mNumFaces; ++i)
    {
        aiFace& face = aimesh->mFaces[i];
        mesh->indices.emplace_back(face.mIndices[0]);
        mesh->indices.emplace_back(face.mIndices[1]);
        mesh->indices.emplace_back(face.mIndices[2]);
    }

    mesh->materialIdx = aimesh->mMaterialIndex;

    return mesh;
}
