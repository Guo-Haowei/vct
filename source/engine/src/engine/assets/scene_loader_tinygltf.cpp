#include "scene_loader_tinygltf.h"

#include "scene/scene.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NOEXCEPTION

#pragma warning(push)
#pragma warning(disable : 4018)
#pragma warning(disable : 4267)
#include "tinygltf/tiny_gltf.h"
#pragma warning(pop)

namespace tinygltf {

bool dummy_load_image(Image*,
                      const int,
                      std::string*,
                      std::string*,
                      int,
                      int,
                      const unsigned char*,
                      int,
                      void*) {
    return true;
}

bool dummy_write_image(const std::string*,
                       const std::string*,
                       const Image*,
                       bool,
                       const URICallbacks*,
                       std::string*,
                       void*) {
    return true;
}

}  // namespace tinygltf

namespace vct {

void SceneLoaderTinyGLTF::process_node(int node_index, ecs::Entity parent) {
    if (node_index < 0 || m_entity_map.count(node_index)) {
        return;
    }

    ecs::Entity entity;
    auto& node = m_model->nodes[node_index];

    if (node.mesh >= 0) {
        DEV_ASSERT(node.mesh < (int)m_scene.get_count<MeshComponent>());
        if (node.skin >= 0) {  // this node is an armature
            entity = m_scene.get_entity<ArmatureComponent>(node.skin);
            MeshComponent* mesh = &m_scene.get_component_array<MeshComponent>()[node.mesh];
            ecs::Entity meshID = m_scene.get_entity<MeshComponent>(node.mesh);
            DEV_ASSERT(!mesh->joints_0.empty());
            if (mesh->armatureID.IsValid()) {
                // Reuse mesh with different skin is not possible currently, so we create a new one:
                LOG_WARN("Re-use mesh for different skin!");
                meshID = entity;
                MeshComponent& newMesh = m_scene.create<MeshComponent>(meshID);
                newMesh = m_scene.get_component_array<MeshComponent>()[node.mesh];
                mesh = &newMesh;
            }
            mesh->armatureID = entity;

            // the object component will use an identity transform but will be parented to the armature
            ecs::Entity objectID = m_scene.create_object_entity("Animated::" + node.name);
            ObjectComponent& object = *m_scene.get_component<ObjectComponent>(objectID);
            object.meshID = meshID;
            m_scene.attach_component(objectID, entity);
        } else {  // this node is a mesh instance
            entity = m_scene.create_object_entity("Object::" + node.name);
            ObjectComponent& object = *m_scene.get_component<ObjectComponent>(entity);
            object.meshID = m_scene.get_entity<MeshComponent>(node.mesh);
        }
    } else if (node.camera >= 0) {
        LOG_WARN("@TODO: camera");
    }

    // light

    // transform
    if (!entity.IsValid()) {
        entity = ecs::Entity::create();
        m_scene.create<TransformComponent>(entity);
        m_scene.create<TagComponent>(entity).GetTagRef() = "Transform::" + node.name;
    }

    m_entity_map[node_index] = entity;

    TransformComponent& transform = *m_scene.get_component<TransformComponent>(entity);
    if (!node.matrix.empty()) {
        mat4 matrix;
        matrix[0].x = float(node.matrix.at(0));
        matrix[0].y = float(node.matrix.at(1));
        matrix[0].z = float(node.matrix.at(2));
        matrix[0].w = float(node.matrix.at(3));
        matrix[1].x = float(node.matrix.at(4));
        matrix[1].y = float(node.matrix.at(5));
        matrix[1].z = float(node.matrix.at(6));
        matrix[1].w = float(node.matrix.at(7));
        matrix[2].x = float(node.matrix.at(8));
        matrix[2].y = float(node.matrix.at(9));
        matrix[2].z = float(node.matrix.at(10));
        matrix[2].w = float(node.matrix.at(11));
        matrix[3].x = float(node.matrix.at(12));
        matrix[3].y = float(node.matrix.at(13));
        matrix[3].z = float(node.matrix.at(14));
        matrix[3].w = float(node.matrix.at(15));
        transform.matrix_transform(matrix);
    } else {
        if (!node.scale.empty()) {
            // Note: limiting min scale because scale <= 0.0001 will break matrix decompose and mess up the model (float precision issue?)
            for (int idx = 0; idx < 3; ++idx) {
                if (std::abs(node.scale[idx]) <= 0.0001) {
                    const double sign = node.scale[idx] < 0 ? -1 : 1;
                    node.scale[idx] = 0.0001001 * sign;
                }
            }
            transform.set_scale(vec3(float(node.scale[0]), float(node.scale[1]), float(node.scale[2])));
        }
        if (!node.rotation.empty()) {
            transform.set_rotation(vec4(float(node.rotation[0]), float(node.rotation[1]), float(node.rotation[2]), float(node.rotation[3])));
        }
        if (!node.translation.empty()) {
            transform.set_translation(vec3(float(node.translation[0]), float(node.translation[1]), float(node.translation[2])));
        }
    }
    transform.update_transform();

    if (parent.IsValid()) {
        m_scene.attach_component(entity, parent);
    }

    for (int child : node.children) {
        process_node(child, entity);
    }
}

bool SceneLoaderTinyGLTF::import_impl() {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    m_model = std::make_shared<tinygltf::Model>();

    loader.SetImageLoader(tinygltf::dummy_load_image, nullptr);
    loader.SetImageWriter(tinygltf::dummy_write_image, nullptr);
    bool ret = loader.LoadASCIIFromFile(m_model.get(), &err, &warn, m_file_path);

    if (!warn.empty()) {
        m_error = std::format("Warn: failed to import scene '{}'\n\tdetails: {}", m_file_path, warn);
        return false;
    }

    if (!err.empty()) {
        m_error = std::format("Error: failed to import scene '{}'\n\tdetails: {}", m_file_path, err);
        return false;
    }

    if (!ret) {
        m_error = std::format("Error: failed to import scene '{}'", m_file_path);
        return false;
    }

    ecs::Entity root = ecs::Entity::create();
    m_scene.create<TransformComponent>(root);
    m_scene.create<TagComponent>(root).GetTagRef() = std::filesystem::path(m_file_path).filename().string();
    m_scene.m_root = root;

    // Create materials
    for (const auto& x : m_model->materials) {
        ecs::Entity materialEntity = m_scene.create_material_entity(x.name);
        MaterialComponent& material = *m_scene.get_component<MaterialComponent>(materialEntity);

        // metallic-roughness workflow:
        auto baseColorTexture = x.values.find("baseColorTexture");
        auto metallicRoughnessTexture = x.values.find("metallicRoughnessTexture");
        auto baseColorFactor = x.values.find("baseColorFactor");
        auto roughnessFactor = x.values.find("roughnessFactor");
        auto metallicFactor = x.values.find("metallicFactor");

        // common workflow:
        auto normalTexture = x.additionalValues.find("normalTexture");
        auto emissiveTexture = x.additionalValues.find("emissiveTexture");
        auto occlusionTexture = x.additionalValues.find("occlusionTexture");
        auto emissiveFactor = x.additionalValues.find("emissiveFactor");
        auto alphaCutoff = x.additionalValues.find("alphaCutoff");
        auto alphaMode = x.additionalValues.find("alphaMode");

        if (baseColorTexture != x.values.end()) {
            auto& tex = m_model->textures[baseColorTexture->second.TextureIndex()];
            int img_source = tex.source;
            if (tex.extensions.count("KHR_texture_basisu")) {
                img_source = tex.extensions["KHR_texture_basisu"].Get("source").Get<int>();
            }
            auto& img = m_model->images[img_source];
            material.mTextures[MaterialComponent::Base].name = m_search_path + img.uri;

            // @TODO:
            // material.mTextures[MaterialComponent::Base].image = g_assetManager->LoadImageSync(searchName);
            // material.textures[MaterialComponent::BASE_COLOR_MAP].uvset = baseColorTexture->second.TextureTexCoord();
        }
        if (normalTexture != x.additionalValues.end()) {
            auto& tex = m_model->textures[normalTexture->second.TextureIndex()];
            int img_source = tex.source;
            if (tex.extensions.count("KHR_texture_basisu")) {
                img_source = tex.extensions["KHR_texture_basisu"].Get("source").Get<int>();
            }
            auto& img = m_model->images[img_source];
            material.mTextures[MaterialComponent::Normal].name = m_search_path + img.uri;
            // material.mTextures[MaterialComponent::Normal].image = g_assetManager->LoadImageSync(searchName);
            //  material.textures[MaterialComponent::NORMAL_MAP].uvset = normalTexture->second.TextureTexCoord();
        }
        if (metallicRoughnessTexture != x.values.end()) {
            auto& tex = m_model->textures[metallicRoughnessTexture->second.TextureIndex()];
            int img_source = tex.source;
            if (tex.extensions.count("KHR_texture_basisu")) {
                img_source = tex.extensions["KHR_texture_basisu"].Get("source").Get<int>();
            }
            auto& img = m_model->images[img_source];
            material.mTextures[MaterialComponent::MetallicRoughness].name = m_search_path + img.uri;
            // material.mTextures[MaterialComponent::MetallicRoughness].resource = ;
        }
#if 0
		if (emissiveTexture != x.additionalValues.end())
		{
			auto& tex = state.gltfModel.textures[emissiveTexture->second.TextureIndex()];
			int img_source = tex.source;
			if (tex.extensions.count("KHR_texture_basisu"))
			{
				img_source = tex.extensions["KHR_texture_basisu"].Get("source").Get<int>();
			}
			auto& img = state.gltfModel.images[img_source];
			material.textures[MaterialComponent::EMISSIVEMAP].resource = wi::resourcemanager::Load(img.uri);
			material.textures[MaterialComponent::EMISSIVEMAP].name = img.uri;
			material.textures[MaterialComponent::EMISSIVEMAP].uvset = emissiveTexture->second.TextureTexCoord();
		}
		if (occlusionTexture != x.additionalValues.end())
		{
			auto& tex = state.gltfModel.textures[occlusionTexture->second.TextureIndex()];
			int img_source = tex.source;
			if (tex.extensions.count("KHR_texture_basisu"))
			{
				img_source = tex.extensions["KHR_texture_basisu"].Get("source").Get<int>();
			}
			auto& img = state.gltfModel.images[img_source];
			material.textures[MaterialComponent::OCCLUSIONMAP].resource = wi::resourcemanager::Load(img.uri);
			material.textures[MaterialComponent::OCCLUSIONMAP].name = img.uri;
			material.textures[MaterialComponent::OCCLUSIONMAP].uvset = occlusionTexture->second.TextureTexCoord();
			material.SetOcclusionEnabled_Secondary(true);
		}
#endif
    }

    // Create meshes:
    for (const auto& gltfMesh : m_model->meshes) {
        ecs::Entity meshEntity = m_scene.create_mesh_entity(gltfMesh.name);
        // m_scene.Component_Attach(meshEntity, state.rootEntity);
        MeshComponent& mesh = *m_scene.get_component<MeshComponent>(meshEntity);

        for (const auto& prim : gltfMesh.primitives) {
            mesh.subsets.push_back(MeshComponent::MeshSubset());
            if (m_scene.get_count<MaterialComponent>() == 0) {
                LOG_FATAL("No material! Consider use default");
            }
            mesh.subsets.back().materialID = m_scene.get_entity<MaterialComponent>(glm::max(0, prim.material));

            const size_t index_remap[] = { 0, 1, 2 };
            uint32_t vertexOffset = (uint32_t)mesh.normals.size();

            if (prim.indices >= 0) {
                // Fill indices:
                const tinygltf::Accessor& accessor = m_model->accessors[prim.indices];
                const tinygltf::BufferView& bufferView = m_model->bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = m_model->buffers[bufferView.buffer];

                int stride = accessor.ByteStride(bufferView);
                size_t indexCount = accessor.count;
                size_t indexOffset = mesh.indices.size();
                mesh.indices.resize(indexOffset + indexCount);
                mesh.subsets.back().indexOffset = (uint32_t)indexOffset;
                mesh.subsets.back().indexCount = (uint32_t)indexCount;

                const uint8_t* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;

                if (stride == 1) {
                    for (size_t i = 0; i < indexCount; i += 3) {
                        mesh.indices[indexOffset + i + 0] = vertexOffset + data[i + 0];
                        mesh.indices[indexOffset + i + 1] = vertexOffset + data[i + 1];
                        mesh.indices[indexOffset + i + 2] = vertexOffset + data[i + 2];
                    }
                } else if (stride == 2) {
                    for (size_t i = 0; i < indexCount; i += 3) {
                        mesh.indices[indexOffset + i + 0] = vertexOffset + ((uint16_t*)data)[i + 0];
                        mesh.indices[indexOffset + i + 1] = vertexOffset + ((uint16_t*)data)[i + 1];
                        mesh.indices[indexOffset + i + 2] = vertexOffset + ((uint16_t*)data)[i + 2];
                    }
                } else if (stride == 4) {
                    for (size_t i = 0; i < indexCount; i += 3) {
                        mesh.indices[indexOffset + i + 0] = vertexOffset + ((uint32_t*)data)[i + 0];
                        mesh.indices[indexOffset + i + 1] = vertexOffset + ((uint32_t*)data)[i + 1];
                        mesh.indices[indexOffset + i + 2] = vertexOffset + ((uint32_t*)data)[i + 2];
                    }
                } else {
                    CRASH_NOW_MSG("unsupported index stride!");
                }
            }

            for (auto& attr : prim.attributes) {
                const std::string& attrName = attr.first;
                int attrData = attr.second;

                const tinygltf::Accessor& accessor = m_model->accessors[attrData];
                const tinygltf::BufferView& bufferView = m_model->bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = m_model->buffers[bufferView.buffer];

                int stride = accessor.ByteStride(bufferView);
                size_t vertexCount = accessor.count;

                if (mesh.subsets.back().indexCount == 0) {
                    CRASH_NOW_MSG("This is not common");
                }

                const uint8_t* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;

                if (attrName == "POSITION") {
                    mesh.positions.resize(vertexOffset + vertexCount);
                    for (size_t i = 0; i < vertexCount; ++i) {
                        mesh.positions[vertexOffset + i] = *(const vec3*)(data + i * stride);
                    }

                    if (accessor.sparse.isSparse) {
                        auto& sparse = accessor.sparse;
                        const tinygltf::BufferView& sparse_indices_view = m_model->bufferViews[sparse.indices.bufferView];
                        const tinygltf::BufferView& sparse_values_view = m_model->bufferViews[sparse.values.bufferView];
                        const tinygltf::Buffer& sparse_indices_buffer = m_model->buffers[sparse_indices_view.buffer];
                        const tinygltf::Buffer& sparse_values_buffer = m_model->buffers[sparse_values_view.buffer];
                        const uint8_t* sparse_indices_data = sparse_indices_buffer.data.data() + sparse.indices.byteOffset + sparse_indices_view.byteOffset;
                        const uint8_t* sparse_values_data = sparse_values_buffer.data.data() + sparse.values.byteOffset + sparse_values_view.byteOffset;
                        switch (sparse.indices.componentType) {
                            default:
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                                for (int s = 0; s < sparse.count; ++s) {
                                    mesh.positions[sparse_indices_data[s]] = ((const vec3*)sparse_values_data)[s];
                                }
                                break;
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                                for (int s = 0; s < sparse.count; ++s) {
                                    mesh.positions[((const uint16_t*)sparse_indices_data)[s]] = ((const vec3*)sparse_values_data)[s];
                                }
                                break;
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                                for (int s = 0; s < sparse.count; ++s) {
                                    mesh.positions[((const uint32_t*)sparse_indices_data)[s]] = ((const vec3*)sparse_values_data)[s];
                                }
                                break;
                        }
                    }
                } else if (attrName == "NORMAL") {
                    mesh.normals.resize(vertexOffset + vertexCount);
                    for (size_t i = 0; i < vertexCount; ++i) {
                        mesh.normals[vertexOffset + i] = *(const vec3*)(data + i * stride);
                    }

                    if (accessor.sparse.isSparse) {
                        auto& sparse = accessor.sparse;
                        const tinygltf::BufferView& sparse_indices_view = m_model->bufferViews[sparse.indices.bufferView];
                        const tinygltf::BufferView& sparse_values_view = m_model->bufferViews[sparse.values.bufferView];
                        const tinygltf::Buffer& sparse_indices_buffer = m_model->buffers[sparse_indices_view.buffer];
                        const tinygltf::Buffer& sparse_values_buffer = m_model->buffers[sparse_values_view.buffer];
                        const uint8_t* sparse_indices_data = sparse_indices_buffer.data.data() + sparse.indices.byteOffset + sparse_indices_view.byteOffset;
                        const uint8_t* sparse_values_data = sparse_values_buffer.data.data() + sparse.values.byteOffset + sparse_values_view.byteOffset;
                        switch (sparse.indices.componentType) {
                            default:
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                                for (int s = 0; s < sparse.count; ++s) {
                                    mesh.normals[sparse_indices_data[s]] = ((const vec3*)sparse_values_data)[s];
                                }
                                break;
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                                for (int s = 0; s < sparse.count; ++s) {
                                    mesh.normals[((const uint16_t*)sparse_indices_data)[s]] = ((const vec3*)sparse_values_data)[s];
                                }
                                break;
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                                for (int s = 0; s < sparse.count; ++s) {
                                    mesh.normals[((const uint32_t*)sparse_indices_data)[s]] = ((const vec3*)sparse_values_data)[s];
                                }
                                break;
                        }
                    }
                } else if (attrName == "TANGENT") {
                    mesh.tangents.resize(vertexOffset + vertexCount);
                    for (size_t i = 0; i < vertexCount; ++i) {
                        mesh.tangents[vertexOffset + i] = *(const vec4*)(data + i * stride);
                    }
                } else if (attrName == "TEXCOORD_0") {
                    mesh.texcoords_0.resize(vertexOffset + vertexCount);
                    if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                        for (size_t i = 0; i < vertexCount; ++i) {
                            const vec2& tex = *(const vec2*)((size_t)data + i * stride);

                            mesh.texcoords_0[vertexOffset + i].x = tex.x;
                            mesh.texcoords_0[vertexOffset + i].y = tex.y;
                        }
                    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                        for (size_t i = 0; i < vertexCount; ++i) {
                            const uint8_t& s = *(uint8_t*)((size_t)data + i * stride + 0);
                            const uint8_t& t = *(uint8_t*)((size_t)data + i * stride + 1);

                            mesh.texcoords_0[vertexOffset + i].x = s / 255.0f;
                            mesh.texcoords_0[vertexOffset + i].y = t / 255.0f;
                        }
                    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                        for (size_t i = 0; i < vertexCount; ++i) {
                            const uint16_t& s = *(uint16_t*)((size_t)data + i * stride + 0 * sizeof(uint16_t));
                            const uint16_t& t = *(uint16_t*)((size_t)data + i * stride + 1 * sizeof(uint16_t));

                            mesh.texcoords_0[vertexOffset + i].x = s / 65535.0f;
                            mesh.texcoords_0[vertexOffset + i].y = t / 65535.0f;
                        }
                    }
                } else if (attrName == "TEXCOORD_1") {
                } else if (attrName == "TEXCOORD_2") {
                } else if (attrName == "TEXCOORD_3") {
                } else if (attrName == "TEXCOORD_4") {
                } else if (attrName == "JOINTS_0") {
                    mesh.joints_0.resize(vertexOffset + vertexCount);
                    if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                        struct JointTmp {
                            uint8_t ind[4];
                        };

                        for (size_t i = 0; i < vertexCount; ++i) {
                            const JointTmp& joint = *(const JointTmp*)(data + i * stride);

                            mesh.joints_0[vertexOffset + i].x = joint.ind[0];
                            mesh.joints_0[vertexOffset + i].y = joint.ind[1];
                            mesh.joints_0[vertexOffset + i].z = joint.ind[2];
                            mesh.joints_0[vertexOffset + i].w = joint.ind[3];
                        }
                    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                        struct JointTmp {
                            uint16_t ind[4];
                        };

                        for (size_t i = 0; i < vertexCount; ++i) {
                            const JointTmp& joint = *(const JointTmp*)(data + i * stride);

                            mesh.joints_0[vertexOffset + i].x = joint.ind[0];
                            mesh.joints_0[vertexOffset + i].y = joint.ind[1];
                            mesh.joints_0[vertexOffset + i].z = joint.ind[2];
                            mesh.joints_0[vertexOffset + i].w = joint.ind[3];
                        }
                    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                        struct JointTmp {
                            uint32_t ind[4];
                        };

                        for (size_t i = 0; i < vertexCount; ++i) {
                            const JointTmp& joint = *(const JointTmp*)(data + i * stride);

                            mesh.joints_0[vertexOffset + i].x = joint.ind[0];
                            mesh.joints_0[vertexOffset + i].y = joint.ind[1];
                            mesh.joints_0[vertexOffset + i].z = joint.ind[2];
                            mesh.joints_0[vertexOffset + i].w = joint.ind[3];
                        }
                    } else {
                        DEV_ASSERT(0);
                    }
                } else if (attrName == "WEIGHTS_0") {
                    mesh.weights_0.resize(vertexOffset + vertexCount);
                    if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                        for (size_t i = 0; i < vertexCount; ++i) {
                            mesh.weights_0[vertexOffset + i] = *(vec4*)((size_t)data + i * stride);
                        }
                    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                        for (size_t i = 0; i < vertexCount; ++i) {
                            const uint8_t& x = *(uint8_t*)((size_t)data + i * stride + 0);
                            const uint8_t& y = *(uint8_t*)((size_t)data + i * stride + 1);
                            const uint8_t& z = *(uint8_t*)((size_t)data + i * stride + 2);
                            const uint8_t& w = *(uint8_t*)((size_t)data + i * stride + 3);

                            mesh.weights_0[vertexOffset + i].x = x / 255.0f;
                            mesh.weights_0[vertexOffset + i].x = y / 255.0f;
                            mesh.weights_0[vertexOffset + i].x = z / 255.0f;
                            mesh.weights_0[vertexOffset + i].x = w / 255.0f;
                        }
                    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                        for (size_t i = 0; i < vertexCount; ++i) {
                            const uint16_t& x = *(uint8_t*)((size_t)data + i * stride + 0 * sizeof(uint16_t));
                            const uint16_t& y = *(uint8_t*)((size_t)data + i * stride + 1 * sizeof(uint16_t));
                            const uint16_t& z = *(uint8_t*)((size_t)data + i * stride + 2 * sizeof(uint16_t));
                            const uint16_t& w = *(uint8_t*)((size_t)data + i * stride + 3 * sizeof(uint16_t));

                            mesh.weights_0[vertexOffset + i].x = x / 65535.0f;
                            mesh.weights_0[vertexOffset + i].x = y / 65535.0f;
                            mesh.weights_0[vertexOffset + i].x = z / 65535.0f;
                            mesh.weights_0[vertexOffset + i].x = w / 65535.0f;
                        }
                    }
                } else if (attrName == "COLOR_0") {
                    LOG_WARN("TODO: COLOR_0");
                } else {
                    LOG_ERROR("Unknown attrib %s", attrName.c_str());
                }
            }

            // TODO: morph target
        }

        // iterate through weights
        if (mesh.normals.empty()) {
            CRASH_NOW_MSG("No normal detected");
        }
    }

    // Create armatures
    for (const auto& skin : m_model->skins) {
        ecs::Entity armatureID = ecs::Entity::create();
        m_scene.create<TagComponent>(armatureID).GetTagRef() = skin.name;
        m_scene.create<TransformComponent>(armatureID);
        ArmatureComponent& armature = m_scene.create<ArmatureComponent>(armatureID);
        if (skin.inverseBindMatrices >= 0) {
            const tinygltf::Accessor& accessor = m_model->accessors[skin.inverseBindMatrices];
            const tinygltf::BufferView& bufferView = m_model->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = m_model->buffers[bufferView.buffer];
            armature.inverseBindMatrices.resize(accessor.count);
            memcpy(armature.inverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(mat4));
        } else {
            LOG_FATAL("No inverse matrices found");
        }
    }

    // Create transform hierarchy, assign objects, meshes, armatures, cameras
    DEV_ASSERT(m_model->scenes.size());
    const tinygltf::Scene& gltfScene = m_model->scenes[glm::max(0, m_model->defaultScene)];
    for (size_t i = 0; i < gltfScene.nodes.size(); ++i) {
        process_node(gltfScene.nodes[i], root);
    }

    // Create armature-bone mappings:
    int armatureIndex = 0;
    for (const auto& skin : m_model->skins) {
        ecs::Entity armatureID = m_scene.get_entity<ArmatureComponent>(armatureIndex);
        ArmatureComponent& armature = m_scene.get_component_array<ArmatureComponent>()[armatureIndex++];

        const size_t jointCount = skin.joints.size();
        armature.boneCollection.resize(jointCount);

        // create bone collection
        for (size_t i = 0; i < jointCount; ++i) {
            int jointIndex = skin.joints[i];
            ecs::Entity boneID = m_entity_map[jointIndex];
            armature.boneCollection[i] = boneID;
        }
    }

    // Create animations:
    for (const auto& anim : m_model->animations) {
        static int animCounter = 0;
        ecs::Entity entity = ecs::Entity::create();
        m_scene.create<TagComponent>(entity).GetTagRef() = anim.name.empty() ? std::string("Untitled") + std::to_string(++animCounter) : anim.name;
        // m_scene.Component_Attach(entity, m_scene.m_root);
        AnimationComponent& animation = m_scene.create<AnimationComponent>(entity);
        animation.samplers.resize(anim.samplers.size());
        animation.channels.resize(anim.channels.size());
        DEV_ASSERT(anim.samplers.size() == anim.channels.size());

        for (size_t i = 0; i < anim.samplers.size(); ++i) {
            const auto& gltfSampler = anim.samplers[i];
            DEV_ASSERT(gltfSampler.interpolation == "LINEAR");
            auto& sampler = animation.samplers[i];

            // Animation Sampler input = keyframe times
            {
                const tinygltf::Accessor& accessor = m_model->accessors[gltfSampler.input];
                const tinygltf::BufferView& bufferView = m_model->bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = m_model->buffers[bufferView.buffer];

                DEV_ASSERT(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

                int stride = accessor.ByteStride(bufferView);
                size_t count = accessor.count;

                sampler.keyframeTimes.resize(count);

                const unsigned char* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;

                DEV_ASSERT(stride == 4);

                for (size_t j = 0; j < count; ++j) {
                    float time = ((float*)data)[j];
                    sampler.keyframeTimes[j] = time;
                    animation.start = glm::min(animation.start, time);
                    animation.end = glm::max(animation.end, time);
                }
            }

            // Animation Sampler output = keyframe data
            {
                const tinygltf::Accessor& accessor = m_model->accessors[gltfSampler.output];
                const tinygltf::BufferView& bufferView = m_model->bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = m_model->buffers[bufferView.buffer];

                int stride = accessor.ByteStride(bufferView);
                size_t count = accessor.count;

                const unsigned char* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;

                switch (accessor.type) {
                    case TINYGLTF_TYPE_SCALAR:
                        DEV_ASSERT(stride == sizeof(float));
                        break;
                    case TINYGLTF_TYPE_VEC3:
                        DEV_ASSERT(stride == sizeof(vec3));
                        break;
                    case TINYGLTF_TYPE_VEC4:
                        DEV_ASSERT(stride == sizeof(vec4));
                        break;
                    default:
                        LOG_FATAL("Invalid format {}", accessor.type);
                        break;
                }
                sampler.keyframeData.resize(count * stride / sizeof(float));
                memcpy(sampler.keyframeData.data(), data, count * stride);
            }
        }

        for (size_t i = 0; i < anim.channels.size(); ++i) {
            const auto& channel = anim.channels[i];
            animation.channels[i].targetID = m_entity_map[channel.target_node];
            DEV_ASSERT(channel.sampler >= 0);
            animation.channels[i].samplerIndex = (uint32_t)channel.sampler;

            if (channel.target_path == "scale") {
                animation.channels[i].path = AnimationComponent::Channel::SCALE;
            } else if (channel.target_path == "rotation") {
                animation.channels[i].path = AnimationComponent::Channel::ROTATION;
            } else if (channel.target_path == "translation") {
                animation.channels[i].path = AnimationComponent::Channel::TRANSLATION;
            } else {
                LOG_WARN("Unkown target path {}", channel.target_path.c_str());
                animation.channels[i].path = AnimationComponent::Channel::UNKNOWN;
            }
        }
    }

    // Create lights:
    // Create cameras:

    m_scene.m_root = root;
    return true;
}

}  // namespace vct

auto load_scene_tinygltf(const std::string& asset_path, void* data) -> std::expected<void, std::string> {
    DEV_ASSERT(data);
    auto scene = (reinterpret_cast<vct::Scene*>(data));
    vct::SceneLoaderTinyGLTF loader(*scene, asset_path);
    return loader.import();
}
