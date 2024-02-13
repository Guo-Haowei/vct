#include "scene_importer_tinygltf.h"

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

void SceneImporterTinyGLTF::process_node(int node_index, ecs::Entity parent) {
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
            ecs::Entity mesh_id = m_scene.get_entity<MeshComponent>(node.mesh);
            DEV_ASSERT(!mesh->joints_0.empty());
            if (mesh->armature_id.is_valid()) {
                // Reuse mesh with different skin is not possible currently, so we create a new one:
                LOG_WARN("Re-use mesh for different skin!");
                mesh_id = entity;
                MeshComponent& newMesh = m_scene.create<MeshComponent>(mesh_id);
                newMesh = m_scene.get_component_array<MeshComponent>()[node.mesh];
                mesh = &newMesh;
            }
            mesh->armature_id = entity;

            // the object component will use an identity transform but will be parented to the armature
            ecs::Entity objectID = m_scene.create_object_entity("Animated::" + node.name);
            ObjectComponent& object = *m_scene.get_component<ObjectComponent>(objectID);
            object.mesh_id = mesh_id;
            m_scene.attach_component(objectID, entity);
        } else {  // this node is a mesh instance
            entity = m_scene.create_object_entity("Object::" + node.name);
            ObjectComponent& object = *m_scene.get_component<ObjectComponent>(entity);
            object.mesh_id = m_scene.get_entity<MeshComponent>(node.mesh);
        }
    } else if (node.camera >= 0) {
        LOG_WARN("@TODO: camera");
    }

    // light

    // transform
    if (!entity.is_valid()) {
        entity = ecs::Entity::create();
        m_scene.create<TransformComponent>(entity);
        m_scene.create<NameComponent>(entity).set_name("Transform::" + node.name);
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

    if (parent.is_valid()) {
        m_scene.attach_component(entity, parent);
    }

    for (int child : node.children) {
        process_node(child, entity);
    }
}

bool SceneImporterTinyGLTF::import_impl() {
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
    m_scene.create<NameComponent>(root).set_name(m_scene_name);
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
            material.textures[MaterialComponent::TEXTURE_BASE].name = m_search_path + img.uri;

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
            material.textures[MaterialComponent::TEXTURE_NORMAL].name = m_search_path + img.uri;
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
            material.textures[MaterialComponent::TEXTURE_METALLIC_ROUGHNESS].name = m_search_path + img.uri;
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
    for (int id = 0; id < (int)m_model->meshes.size(); ++id) {
        const tinygltf::Mesh& mesh = m_model->meshes[id];
        process_mesh(mesh, id);
    }
    // Create armatures
    for (const auto& skin : m_model->skins) {
        ecs::Entity armature_id = ecs::Entity::create();
        m_scene.create<NameComponent>(armature_id).set_name(skin.name);
        m_scene.create<TransformComponent>(armature_id);
        ArmatureComponent& armature = m_scene.create<ArmatureComponent>(armature_id);
        if (skin.inverseBindMatrices >= 0) {
            const tinygltf::Accessor& accessor = m_model->accessors[skin.inverseBindMatrices];
            const tinygltf::BufferView& bufferView = m_model->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = m_model->buffers[bufferView.buffer];
            armature.inverse_bind_matrices.resize(accessor.count);
            memcpy(armature.inverse_bind_matrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(mat4));
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
        ecs::Entity armature_id = m_scene.get_entity<ArmatureComponent>(armatureIndex);
        ArmatureComponent& armature = m_scene.get_component_array<ArmatureComponent>()[armatureIndex++];

        const size_t jointCount = skin.joints.size();
        armature.bone_collection.resize(jointCount);

        // create bone collection
        for (size_t i = 0; i < jointCount; ++i) {
            int jointIndex = skin.joints[i];
            ecs::Entity boneID = m_entity_map[jointIndex];
            armature.bone_collection[i] = boneID;
        }
    }

    // Create animations:
    for (int id = 0; id < (int)m_model->animations.size(); ++id) {
        const tinygltf::Animation& anim = m_model->animations[id];
        process_animation(anim, id);
    }
    // Create lights:
    // Create cameras:

    m_scene.m_root = root;
    return true;
}

void SceneImporterTinyGLTF::process_mesh(const tinygltf::Mesh& gltf_mesh, int) {
    ecs::Entity mesh_id = m_scene.create_mesh_entity("Mesh::" + gltf_mesh.name);
    // m_scene.Component_Attach(mesh_id, state.rootEntity);
    MeshComponent& mesh = *m_scene.get_component<MeshComponent>(mesh_id);

    for (const auto& prim : gltf_mesh.primitives) {
        MeshComponent::MeshSubset subset;
        if (m_scene.get_count<MaterialComponent>() == 0) {
            LOG_FATAL("No material! Consider use default");
        }
        subset.material_id = m_scene.get_entity<MaterialComponent>(glm::max(0, prim.material));

        const size_t index_remap[] = { 0, 1, 2 };
        uint32_t vertexOffset = (uint32_t)mesh.normals.size();

        if (prim.indices >= 0) {
            // Fill indices:
            const tinygltf::Accessor& accessor = m_model->accessors[prim.indices];
            const tinygltf::BufferView& bufferView = m_model->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = m_model->buffers[bufferView.buffer];

            int stride = accessor.ByteStride(bufferView);
            size_t index_count = accessor.count;
            size_t index_offset = mesh.indices.size();
            mesh.indices.resize(index_offset + index_count);
            subset.index_offset = (uint32_t)index_offset;
            subset.index_count = (uint32_t)index_count;
            mesh.subsets.emplace_back(subset);

            const uint8_t* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;

            if (stride == 1) {
                for (size_t index = 0; index < index_count; index += 3) {
                    mesh.indices[index_offset + index + 0] = vertexOffset + data[index + 0];
                    mesh.indices[index_offset + index + 1] = vertexOffset + data[index + 1];
                    mesh.indices[index_offset + index + 2] = vertexOffset + data[index + 2];
                }
            } else if (stride == 2) {
                for (size_t index = 0; index < index_count; index += 3) {
                    mesh.indices[index_offset + index + 0] = vertexOffset + ((uint16_t*)data)[index + 0];
                    mesh.indices[index_offset + index + 1] = vertexOffset + ((uint16_t*)data)[index + 1];
                    mesh.indices[index_offset + index + 2] = vertexOffset + ((uint16_t*)data)[index + 2];
                }
            } else if (stride == 4) {
                for (size_t index = 0; index < index_count; index += 3) {
                    mesh.indices[index_offset + index + 0] = vertexOffset + ((uint32_t*)data)[index + 0];
                    mesh.indices[index_offset + index + 1] = vertexOffset + ((uint32_t*)data)[index + 1];
                    mesh.indices[index_offset + index + 2] = vertexOffset + ((uint32_t*)data)[index + 2];
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

            if (mesh.subsets.back().index_count == 0) {
                CRASH_NOW_MSG("This is not common");
            }

            const uint8_t* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;

            if (attrName == "POSITION") {
                mesh.positions.resize(vertexOffset + vertexCount);
                for (size_t index = 0; index < vertexCount; ++index) {
                    mesh.positions[vertexOffset + index] = *(const vec3*)(data + index * stride);
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
                for (size_t index = 0; index < vertexCount; ++index) {
                    mesh.normals[vertexOffset + index] = *(const vec3*)(data + index * stride);
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
                for (size_t index = 0; index < vertexCount; ++index) {
                    mesh.tangents[vertexOffset + index] = *(const vec4*)(data + index * stride);
                }
            } else if (attrName == "TEXCOORD_0") {
                mesh.texcoords_0.resize(vertexOffset + vertexCount);
                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                    for (size_t index = 0; index < vertexCount; ++index) {
                        const vec2& tex = *(const vec2*)((size_t)data + index * stride);

                        mesh.texcoords_0[vertexOffset + index].x = tex.x;
                        mesh.texcoords_0[vertexOffset + index].y = tex.y;
                    }
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                    for (size_t index = 0; index < vertexCount; ++index) {
                        const uint8_t& s = *(uint8_t*)((size_t)data + index * stride + 0);
                        const uint8_t& t = *(uint8_t*)((size_t)data + index * stride + 1);

                        mesh.texcoords_0[vertexOffset + index].x = s / 255.0f;
                        mesh.texcoords_0[vertexOffset + index].y = t / 255.0f;
                    }
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                    for (size_t index = 0; index < vertexCount; ++index) {
                        const uint16_t& s = *(uint16_t*)((size_t)data + index * stride + 0 * sizeof(uint16_t));
                        const uint16_t& t = *(uint16_t*)((size_t)data + index * stride + 1 * sizeof(uint16_t));

                        mesh.texcoords_0[vertexOffset + index].x = s / 65535.0f;
                        mesh.texcoords_0[vertexOffset + index].y = t / 65535.0f;
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

                    for (size_t index = 0; index < vertexCount; ++index) {
                        const JointTmp& joint = *(const JointTmp*)(data + index * stride);

                        mesh.joints_0[vertexOffset + index].x = joint.ind[0];
                        mesh.joints_0[vertexOffset + index].y = joint.ind[1];
                        mesh.joints_0[vertexOffset + index].z = joint.ind[2];
                        mesh.joints_0[vertexOffset + index].w = joint.ind[3];
                    }
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                    struct JointTmp {
                        uint16_t ind[4];
                    };

                    for (size_t index = 0; index < vertexCount; ++index) {
                        const JointTmp& joint = *(const JointTmp*)(data + index * stride);

                        mesh.joints_0[vertexOffset + index].x = joint.ind[0];
                        mesh.joints_0[vertexOffset + index].y = joint.ind[1];
                        mesh.joints_0[vertexOffset + index].z = joint.ind[2];
                        mesh.joints_0[vertexOffset + index].w = joint.ind[3];
                    }
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                    struct JointTmp {
                        uint32_t ind[4];
                    };

                    for (size_t index = 0; index < vertexCount; ++index) {
                        const JointTmp& joint = *(const JointTmp*)(data + index * stride);

                        mesh.joints_0[vertexOffset + index].x = joint.ind[0];
                        mesh.joints_0[vertexOffset + index].y = joint.ind[1];
                        mesh.joints_0[vertexOffset + index].z = joint.ind[2];
                        mesh.joints_0[vertexOffset + index].w = joint.ind[3];
                    }
                } else {
                    DEV_ASSERT(0);
                }
            } else if (attrName == "WEIGHTS_0") {
                mesh.weights_0.resize(vertexOffset + vertexCount);
                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                    for (size_t index = 0; index < vertexCount; ++index) {
                        mesh.weights_0[vertexOffset + index] = *(vec4*)((size_t)data + index * stride);
                    }
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                    for (size_t index = 0; index < vertexCount; ++index) {
                        const uint8_t& x = *(uint8_t*)((size_t)data + index * stride + 0);
                        const uint8_t& y = *(uint8_t*)((size_t)data + index * stride + 1);
                        const uint8_t& z = *(uint8_t*)((size_t)data + index * stride + 2);
                        const uint8_t& w = *(uint8_t*)((size_t)data + index * stride + 3);

                        mesh.weights_0[vertexOffset + index].x = x / 255.0f;
                        mesh.weights_0[vertexOffset + index].x = y / 255.0f;
                        mesh.weights_0[vertexOffset + index].x = z / 255.0f;
                        mesh.weights_0[vertexOffset + index].x = w / 255.0f;
                    }
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                    for (size_t index = 0; index < vertexCount; ++index) {
                        const uint16_t& x = *(uint8_t*)((size_t)data + index * stride + 0 * sizeof(uint16_t));
                        const uint16_t& y = *(uint8_t*)((size_t)data + index * stride + 1 * sizeof(uint16_t));
                        const uint16_t& z = *(uint8_t*)((size_t)data + index * stride + 2 * sizeof(uint16_t));
                        const uint16_t& w = *(uint8_t*)((size_t)data + index * stride + 3 * sizeof(uint16_t));

                        mesh.weights_0[vertexOffset + index].x = x / 65535.0f;
                        mesh.weights_0[vertexOffset + index].x = y / 65535.0f;
                        mesh.weights_0[vertexOffset + index].x = z / 65535.0f;
                        mesh.weights_0[vertexOffset + index].x = w / 65535.0f;
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

void SceneImporterTinyGLTF::process_animation(const tinygltf::Animation& gltf_anim, int) {
    static int s_counter = 0;

    std::string tag = gltf_anim.name;
    if (tag.empty()) {
        tag = std::format("{}::animation_{}", m_scene_name, ++s_counter);
    }
    auto entity = m_scene.create_name_entity(tag);

    // m_scene.Component_Attach(entity, m_scene.m_root);
    AnimationComponent& animation = m_scene.create<AnimationComponent>(entity);
    animation.samplers.resize(gltf_anim.samplers.size());
    animation.channels.resize(gltf_anim.channels.size());
    DEV_ASSERT(gltf_anim.samplers.size() == gltf_anim.channels.size());

    for (size_t index = 0; index < gltf_anim.samplers.size(); ++index) {
        const auto& gltfSampler = gltf_anim.samplers[index];
        DEV_ASSERT(gltfSampler.interpolation == "LINEAR");
        auto& sampler = animation.samplers[index];

        // Animation Sampler input = keyframe times
        {
            const tinygltf::Accessor& accessor = m_model->accessors[gltfSampler.input];
            const tinygltf::BufferView& bufferView = m_model->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = m_model->buffers[bufferView.buffer];

            DEV_ASSERT(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

            int stride = accessor.ByteStride(bufferView);
            size_t count = accessor.count;

            sampler.keyframe_times.resize(count);

            const unsigned char* data = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;

            DEV_ASSERT(stride == 4);

            for (size_t j = 0; j < count; ++j) {
                float time = ((float*)data)[j];
                sampler.keyframe_times[j] = time;
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
            sampler.keyframe_data.resize(count * stride / sizeof(float));
            memcpy(sampler.keyframe_data.data(), data, count * stride);
        }
    }

    for (size_t index = 0; index < gltf_anim.channels.size(); ++index) {
        const auto& channel = gltf_anim.channels[index];
        animation.channels[index].target_id = m_entity_map[channel.target_node];
        DEV_ASSERT(channel.sampler >= 0);
        animation.channels[index].sampler_index = (uint32_t)channel.sampler;

        if (channel.target_path == "scale") {
            animation.channels[index].path = AnimationComponent::Channel::PATH_SCALE;
        } else if (channel.target_path == "rotation") {
            animation.channels[index].path = AnimationComponent::Channel::PATH_ROTATION;
        } else if (channel.target_path == "translation") {
            animation.channels[index].path = AnimationComponent::Channel::PATH_TRANSLATION;
        } else {
            LOG_WARN("Unkown target path {}", channel.target_path.c_str());
            animation.channels[index].path = AnimationComponent::Channel::PATH_UNKNOWN;
        }
    }
}

}  // namespace vct
