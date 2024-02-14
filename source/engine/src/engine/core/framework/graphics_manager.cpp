#pragma once
#include "graphics_manager.h"

#include <random>

#include "core/math/geometry.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "rendering/render_data.h"
/////
#include "core/base/rid_owner.h"
#include "core/framework/scene_manager.h"
#include "rendering/r_defines.h"
#include "rendering/r_editor.h"
#include "rendering/rendering_dvars.h"
#include "rendering/shader_program_manager.h"
#include "servers/display_server.h"
#include "vsinput.glsl.h"

// @TODO: refactor
#include "rendering/render_graph/render_graph_vxgi.h"

/// textures
GpuTexture g_albedoVoxel;
GpuTexture g_normalVoxel;
MeshData g_box;

// @TODO: fix this
vct::RIDAllocator<MeshData> g_meshes;
vct::RIDAllocator<MaterialData> g_materials;

static GLuint g_noiseTexture;

extern void FillMaterialCB(const MaterialData* mat, MaterialConstantBuffer& cb);

template<typename T>
static void buffer_storage(GLuint buffer, const std::vector<T>& data) {
    glNamedBufferStorage(buffer, sizeof(T) * data.size(), data.data(), 0);
}

static inline void bind_to_slot(GLuint buffer, int slot, int size) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(slot, size, GL_FLOAT, GL_FALSE, size * sizeof(float), 0);
    glEnableVertexAttribArray(slot);
}

namespace vct {

static void APIENTRY gl_debug_callback(GLenum, GLenum, unsigned int, GLenum, GLsizei, const char*, const void*);

bool GraphicsManager::initialize() {
    if (gladLoadGL() == 0) {
        LOG_FATAL("[glad] failed to import gl functions");
        return false;
    }

    LOG_VERBOSE("[opengl] renderer: {}", (const char*)glGetString(GL_RENDERER));
    LOG_VERBOSE("[opengl] version: {}", (const char*)glGetString(GL_VERSION));

    if (DVAR_GET_BOOL(r_gpu_validation)) {
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(gl_debug_callback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
            LOG_VERBOSE("[opengl] debug callback enabled");
        }
    }

    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    createGpuResources();

    g_meshes.set_description("GPU-Mesh-Allocator");
    g_materials.set_description("GPU-Material-Allocator");

    m_render_data = std::make_shared<RenderData>();
    return true;
}

void GraphicsManager::finalize() {
    destroyGpuResources();

    ImGui_ImplOpenGL3_Shutdown();
}

static void create_mesh_data(const MeshComponent& mesh, MeshData& out_mesh) {
    const bool has_normals = !mesh.normals.empty();
    const bool has_uvs = !mesh.texcoords_0.empty();
    const bool has_tangents = !mesh.tangents.empty();
    const bool has_joints = !mesh.joints_0.empty();
    const bool has_weights = !mesh.weights_0.empty();

    int vbo_count = 1 + has_normals + has_uvs + has_tangents + has_joints + has_weights;
    DEV_ASSERT(vbo_count <= array_length(out_mesh.vbos));

    glGenVertexArrays(1, &out_mesh.vao);

    // @TODO: fix this hack
    glGenBuffers(1, &out_mesh.ebo);
    glGenBuffers(6, out_mesh.vbos);

    int slot = -1;
    glBindVertexArray(out_mesh.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_mesh.ebo);

    slot = get_position_slot();
    // @TODO: refactor these
    bind_to_slot(out_mesh.vbos[slot], slot, 3);
    buffer_storage(out_mesh.vbos[slot], mesh.positions);

    if (has_normals) {
        slot = get_normal_slot();
        bind_to_slot(out_mesh.vbos[slot], slot, 3);
        buffer_storage(out_mesh.vbos[slot], mesh.normals);
    }
    if (has_uvs) {
        slot = get_uv_slot();
        bind_to_slot(out_mesh.vbos[slot], slot, 2);
        buffer_storage(out_mesh.vbos[slot], mesh.texcoords_0);
    }
    if (has_tangents) {
        slot = get_tangent_slot();
        bind_to_slot(out_mesh.vbos[slot], slot, 3);
        buffer_storage(out_mesh.vbos[slot], mesh.tangents);
    }
    if (has_joints) {
        slot = get_bone_id_slot();
        bind_to_slot(out_mesh.vbos[slot], slot, 4);
        buffer_storage(out_mesh.vbos[slot], mesh.joints_0);
        DEV_ASSERT(!mesh.weights_0.empty());
        slot = get_bone_weight_slot();
        bind_to_slot(out_mesh.vbos[slot], slot, 4);
        buffer_storage(out_mesh.vbos[slot], mesh.weights_0);
    }

    buffer_storage(out_mesh.ebo, mesh.indices);
    out_mesh.count = static_cast<uint32_t>(mesh.indices.size());

    glBindVertexArray(0);
}

void GraphicsManager::event_received(std::shared_ptr<Event> event) {
    SceneChangeEvent* e = dynamic_cast<SceneChangeEvent*>(event.get());
    if (!e) {
        return;
    }

    const Scene& scene = *e->get_scene();
    for (size_t idx = 0; idx < scene.get_count<MeshComponent>(); ++idx) {
        const MeshComponent& mesh = scene.get_component_array<MeshComponent>()[idx];
        if (mesh.gpu_resource.is_valid()) {
            ecs::Entity entity = scene.get_entity<MeshComponent>(idx);
            const NameComponent& name = *scene.get_component<NameComponent>(entity);
            LOG_WARN("[begin_scene] mesh '{}' (idx: {}) already has gpu resource", name.get_name(), idx);
            continue;
        }
        RID rid = g_meshes.make_rid();
        mesh.gpu_resource = rid;
        create_mesh_data(mesh, *g_meshes.get_or_null(rid));
    }

    // create material
    DEV_ASSERT(scene.get_count<MaterialComponent>() < array_length(g_constantCache.cache.c_albedo_maps));

    for (int idx = 0; idx < scene.get_count<MaterialComponent>(); ++idx) {
        const MaterialComponent& material_component = scene.get_component_array<MaterialComponent>()[idx];
        if (material_component.gpu_resource.is_valid()) {
            ecs::Entity entity = scene.get_entity<MaterialComponent>(idx);
            const NameComponent& name = *scene.get_component<NameComponent>(entity);
            LOG_WARN("[begin_scene] material '{}' (idx: {}) already has gpu resource", name.get_name(), idx);
            continue;
        }

        RID rid = g_materials.make_rid();
        material_component.gpu_resource = rid;
        MaterialData* material = g_materials.get_or_null(rid);
        DEV_ASSERT(material);

        {
            const std::string& textureMap = material_component.textures[MaterialComponent::TEXTURE_BASE].name;
            material->albedoColor = material_component.base_color;
            if (!textureMap.empty()) {
                material->albedoMap.create_texture2d_from_image(textureMap);
                g_constantCache.cache.c_albedo_maps[idx].data = gl::MakeTextureResident(material->albedoMap.GetHandle());
            }
        }

        {
            const std::string& textureMap = material_component.textures[MaterialComponent::TEXTURE_METALLIC_ROUGHNESS].name;
            material->metallic = material_component.metallic;
            material->roughness = material_component.roughness;
            if (!textureMap.empty()) {
                material->materialMap.create_texture2d_from_image(textureMap);
                g_constantCache.cache.c_pbr_maps[idx].data = gl::MakeTextureResident(material->materialMap.GetHandle());
            }
        }

        {
            const std::string& textureMap = material_component.textures[MaterialComponent::TEXTURE_NORMAL].name;
            if (!textureMap.empty()) {
                material->normalMap.create_texture2d_from_image(textureMap);
                g_constantCache.cache.c_normal_maps[idx].data = gl::MakeTextureResident(material->normalMap.GetHandle());
                // LOG("material has bump {}", mat->normalTexture.c_str());
            }
        }

        material->textureMapIdx = idx;
    }

    g_constantCache.Update();
}

// @TODO: refactor
static void create_ssao_resource() {
    // @TODO: save
    // generate sample kernel
    std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);  // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec4> ssaoKernel;
    const int kernelSize = DVAR_GET_INT(r_ssaoKernelSize);
    for (int i = 0; i < kernelSize; ++i) {
        // [-1, 1], [-1, 1], [0, 1]
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0,
                         randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / kernelSize;

        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.emplace_back(vec4(sample, 0.0f));
    }

    memset(&g_constantCache.cache.c_ssao_kernels, 0, sizeof(g_constantCache.cache.c_ssao_kernels));
    memcpy(&g_constantCache.cache.c_ssao_kernels, ssaoKernel.data(), sizeof(ssaoKernel.front()) * ssaoKernel.size());

    // generate noise texture
    const int noiseSize = DVAR_GET_INT(r_ssaoNoiseSize);

    std::vector<glm::vec3> ssaoNoise;
    for (int i = 0; i < noiseSize * noiseSize; ++i) {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
        noise = glm::normalize(noise);
        ssaoNoise.emplace_back(noise);
    }
    unsigned int noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, noiseSize, noiseSize, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    g_constantCache.cache.c_kernel_noise_map = gl::MakeTextureResident(noiseTexture);
    g_noiseTexture = noiseTexture;
}

void GraphicsManager::createGpuResources() {
    create_ssao_resource();

    R_Alloc_Cbuffers();
    R_CreateEditorResource();

    // create a dummy box data
    create_mesh_data(vct::make_box_mesh(), g_box);

    std::string method(DVAR_GET_STRING(r_render_graph));
    if (method == "vxgi") {
        m_method = RENDER_GRAPH_VXGI;
    } else if (method == "vxgi_debug") {
        m_method = RENDER_GRAPH_VXGI_DEBUG;
    }

    switch (m_method) {
        case vct::GraphicsManager::RENDER_GRAPH_VXGI:
            create_render_graph_vxgi(g_render_graph);
            break;
        case vct::GraphicsManager::RENDER_GRAPH_VXGI_DEBUG:
            create_render_graph_vxgi_debug(g_render_graph);
            break;
        default:
            CRASH_NOW();
            break;
    }

    const int voxelSize = DVAR_GET_INT(r_voxel_size);

    /// create voxel image
    {
        Texture3DCreateInfo info;
        info.wrapS = info.wrapT = info.wrapR = GL_CLAMP_TO_BORDER;
        info.size = voxelSize;
        info.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        info.magFilter = GL_NEAREST;
        info.mipLevel = math::log_two(voxelSize);
        info.format = GL_RGBA16F;

        g_albedoVoxel.create3DEmpty(info);
        g_normalVoxel.create3DEmpty(info);
    }

    // create box quad
    R_CreateQuad();

    auto& cache = g_constantCache.cache;
    cache.c_voxel_map = gl::MakeTextureResident(g_albedoVoxel.GetHandle());
    cache.c_voxel_normal_map = gl::MakeTextureResident(g_normalVoxel.GetHandle());

    // @TODO: refactor
    std::shared_ptr<RenderPass> pass;
    pass = g_render_graph.find_pass(SHADOW_PASS);
    if (pass) {
        cache.c_shadow_map = gl::MakeTextureResident(pass->get_depth_attachment());
    }
    pass = g_render_graph.find_pass(SSAO_PASS);
    if (pass) {
        cache.c_ssao_map = gl::MakeTextureResident(pass->get_color_attachment(0));
    }

    switch (m_method) {
        case vct::GraphicsManager::RENDER_GRAPH_VXGI:
            pass = g_render_graph.find_pass(LIGHTING_PASS);
            break;
        case vct::GraphicsManager::RENDER_GRAPH_VXGI_DEBUG:
            pass = g_render_graph.find_pass(VXGI_DEBUG_PASS);
            break;
        default:
            CRASH_NOW();
            break;
    }
    if (pass) {
        cache.c_fxaa_input_image = gl::MakeTextureResident(pass->get_color_attachment(0));
    }
    pass = g_render_graph.find_pass(FXAA_PASS);
    if (pass) {
        cache.c_fxaa_image = gl::MakeTextureResident(pass->get_color_attachment(0));
    }

    pass = g_render_graph.find_pass(GBUFFER_PASS);
    if (pass) {
        cache.c_gbuffer_depth_map = gl::MakeTextureResident(pass->get_depth_attachment());
        cache.c_gbuffer_position_metallic_map = gl::MakeTextureResident(pass->get_color_attachment(0));
        cache.c_gbuffer_normal_roughness_map = gl::MakeTextureResident(pass->get_color_attachment(1));
        cache.c_gbuffer_albedo_map = gl::MakeTextureResident(pass->get_color_attachment(2));
    }

    g_constantCache.Update();
}

struct MaterialCache {
    vec4 albedo_color;  // if it doesn't have albedo color, then it's alpha is 0.0f
    float metallic = 0.0f;
    float roughness = 0.0f;
    float has_metallic_roughness_texture = 0.0f;
    float has_normal_texture = 0.0f;
    float reflect = 0.0f;

    MaterialCache& operator=(const MaterialData& mat) {
        albedo_color = mat.albedoColor;
        roughness = mat.roughness;
        metallic = mat.metallic;
        reflect = mat.reflectPower;
        has_metallic_roughness_texture = mat.materialMap.GetHandle() == 0 ? 0.0f : 1.0f;
        has_normal_texture = mat.normalMap.GetHandle() == 0 ? 0.0f : 1.0f;

        return *this;
    }
};

uint32_t GraphicsManager::get_final_image() const {
    switch (m_method) {
        case vct::GraphicsManager::RENDER_GRAPH_VXGI:
            return g_render_graph.find_pass(FINAL_PASS)->get_color_attachment(0);
        case vct::GraphicsManager::RENDER_GRAPH_VXGI_DEBUG:
            return g_render_graph.find_pass(FXAA_PASS)->get_color_attachment(0);
        default:
            CRASH_NOW();
            return 0;
    }
}

void GraphicsManager::render() {
    Scene& scene = SceneManager::singleton().get_scene();
    m_render_data->update(&scene);

    g_perFrameCache.Update();
    g_render_graph.execute();
}

void GraphicsManager::destroyGpuResources() {
    R_DestroyEditorResource();
    R_Destroy_Cbuffers();

    glDeleteTextures(1, &g_noiseTexture);
}

static void APIENTRY gl_debug_callback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei,
                                       const char* message, const void*) {
    switch (id) {
        case 131185:
            return;
    }

    const char* sourceStr = "GL_DEBUG_SOURCE_OTHER";
    switch (source) {
        case GL_DEBUG_SOURCE_API:
            sourceStr = "GL_DEBUG_SOURCE_API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            sourceStr = "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            sourceStr = "GL_DEBUG_SOURCE_SHADER_COMPILER";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            sourceStr = "GL_DEBUG_SOURCE_THIRD_PARTY";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            sourceStr = "GL_DEBUG_SOURCE_APPLICATION";
            break;
        default:
            break;
    }

    const char* typeStr = "GL_DEBUG_TYPE_OTHER";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            typeStr = "GL_DEBUG_TYPE_ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            typeStr = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            typeStr = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            typeStr = "GL_DEBUG_TYPE_PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            typeStr = "GL_DEBUG_TYPE_PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_MARKER:
            typeStr = "GL_DEBUG_TYPE_MARKER";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            typeStr = "GL_DEBUG_TYPE_PUSH_GROUP";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            typeStr = "GL_DEBUG_TYPE_POP_GROUP";
            break;
        default:
            break;
    }

    LogLevel level = LOG_LEVEL_NORMAL;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            level = LOG_LEVEL_ERROR;
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            level = LOG_LEVEL_WARN;
            break;
        default:
            break;
    }

    vct::log_impl(level, std::format("[opengl] {}\n\t| id: {} | source: {} | type: {}", message, id, sourceStr, typeStr));
}

}  // namespace vct
