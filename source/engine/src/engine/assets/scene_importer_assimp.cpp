#include "scene_importer_assimp.h"

#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

namespace vct {

bool SceneImporterAssimp::import_impl() {
    Assimp::Importer importer;

    unsigned int flag = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs;

    const aiScene* aiscene = importer.ReadFile(m_file_path, flag);

    // check for errors
    if (!aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiscene->mRootNode)  // if is Not Zero
    {
        m_error = std::format("Error: failed to import scene '{}'\n\tdetails: {}", m_file_path, importer.GetErrorString());
        return false;
    }

    const uint32_t numMeshes = aiscene->mNumMeshes;
    const uint32_t numMaterials = aiscene->mNumMaterials;

    // LOG_VERBOSE("scene '{}' has {} meshes, {} materials", m_file_path, numMeshes, numMaterials);

    for (uint32_t i = 0; i < numMaterials; ++i) {
        process_material(*aiscene->mMaterials[i]);
    }

    for (uint32_t i = 0; i < numMeshes; ++i) {
        process_mesh(*aiscene->mMeshes[i]);
    }

    ecs::Entity root = process_node(aiscene->mRootNode, ecs::Entity::INVALID);
    m_scene.get_component<NameComponent>(root)->set_name(m_scene_name);

    m_scene.m_root = root;
    return true;
}

void SceneImporterAssimp::process_material(aiMaterial& material) {
    auto material_id = m_scene.create_material_entity(std::string("Material::") + material.GetName().C_Str());
    MaterialComponent* materialComponent = m_scene.get_component<MaterialComponent>(material_id);
    DEV_ASSERT(materialComponent);

    auto getMaterialPath = [&](aiTextureType type, unsigned int index) -> std::string {
        aiString path;
        if (material.GetTexture(type, index, &path) == AI_SUCCESS) {
            return m_search_path + path.C_Str();
        }
        return "";
    };

    std::string path = getMaterialPath(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE);
    if (path.empty()) {
        path = getMaterialPath(aiTextureType_DIFFUSE, 0);
    }
    materialComponent->textures[MaterialComponent::TEXTURE_BASE].name = path;

    path = getMaterialPath(aiTextureType_NORMALS, 0);
    if (path.empty()) {
        path = getMaterialPath(aiTextureType_HEIGHT, 0);
    }
    materialComponent->textures[MaterialComponent::TEXTURE_NORMAL].name = path;

    materialComponent->textures[MaterialComponent::TEXTURE_METALLIC_ROUGHNESS].name =
        getMaterialPath(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE);

    m_materials.emplace_back(material_id);
}

void SceneImporterAssimp::process_mesh(const aiMesh& mesh) {
    DEV_ASSERT(mesh.mNumVertices);
    const std::string mesh_name(mesh.mName.C_Str());
    const bool has_uv = mesh.mTextureCoords[0];
    if (!has_uv) {
        LOG_WARN("mesh {} does not have texture coordinates", mesh_name);
    }

    ecs::Entity mesh_id = m_scene.create_mesh_entity("Mesh::" + mesh_name);
    MeshComponent& mesh_component = *m_scene.get_component<MeshComponent>(mesh_id);

    for (uint32_t i = 0; i < mesh.mNumVertices; ++i) {
        auto& position = mesh.mVertices[i];
        mesh_component.positions.emplace_back(vec3(position.x, position.y, position.z));
        auto& normal = mesh.mNormals[i];
        mesh_component.normals.emplace_back(vec3(normal.x, normal.y, normal.z));
        auto& tangent = mesh.mTangents[i];
        mesh_component.tangents.emplace_back(vec3(tangent.x, tangent.y, tangent.z));

        if (has_uv) {
            auto& uv = mesh.mTextureCoords[0][i];
            mesh_component.texcoords_0.emplace_back(vec2(uv.x, uv.y));
        } else {
            mesh_component.texcoords_1.emplace_back(vec2(0));
        }
    }

    for (uint32_t i = 0; i < mesh.mNumFaces; ++i) {
        aiFace& face = mesh.mFaces[i];
        mesh_component.indices.emplace_back(face.mIndices[0]);
        mesh_component.indices.emplace_back(face.mIndices[1]);
        mesh_component.indices.emplace_back(face.mIndices[2]);
    }

    DEV_ASSERT(m_materials.size());
    MeshComponent::MeshSubset subset;
    subset.index_count = (uint32_t)mesh_component.indices.size();
    subset.index_offset = 0;
    subset.material_id = m_materials.at(mesh.mMaterialIndex);
    mesh_component.subsets.emplace_back(subset);

    m_meshes.push_back(mesh_id);
}

ecs::Entity SceneImporterAssimp::process_node(const aiNode* node, ecs::Entity parent) {
    const auto key = std::string(node->mName.C_Str());

    ecs::Entity entity;

    if (node->mNumMeshes == 1) {  // geometry node
        entity = m_scene.create_object_entity("Geometry::" + key);

        ObjectComponent& objComponent = *m_scene.get_component<ObjectComponent>(entity);
        objComponent.mesh_id = m_meshes[node->mMeshes[0]];
    } else {  // else make it a transform/bone node
        entity = m_scene.create_transform_entity("Node::" + key);

        for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
            ecs::Entity child = m_scene.create_object_entity("");
            auto tagComponent = m_scene.get_component<NameComponent>(child);
            tagComponent->set_name("SubGeometry_" + std::to_string(child.get_id()));
            ObjectComponent& objComponent = *m_scene.get_component<ObjectComponent>(child);
            objComponent.mesh_id = m_meshes[node->mMeshes[i]];
            m_scene.attach_component(child, entity);
        }
    }

    DEV_ASSERT(m_scene.contains<TransformComponent>(entity));

    const aiMatrix4x4& local = node->mTransformation;                       // row major matrix
    mat4 localTransformColumnMajor(local.a1, local.b1, local.c1, local.d1,  // x0 y0 z0 w0
                                   local.a2, local.b2, local.c2, local.d2,  // x1 y1 z1 w1
                                   local.a3, local.b3, local.c3, local.d3,  // x2 y2 z2 w2
                                   local.a4, local.b4, local.c4, local.d4   // x3 y3 z3 w3
    );
    TransformComponent& transform = *m_scene.get_component<TransformComponent>(entity);
    transform.matrix_transform(localTransformColumnMajor);

    if (parent.is_valid()) {
        m_scene.attach_component(entity, parent);
    }

    // process children
    for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
        process_node(node->mChildren[childIndex], entity);
    }

    return entity;
}

}  // namespace vct
