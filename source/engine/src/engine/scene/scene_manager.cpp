#include "scene_manager.h"

#include "assets/asset_manager.h"
#include "assets/scene_loader_assimp.h"
#include "core/camera.h"
#include "imgui/imgui.h"
#include "servers/rendering/r_cbuffers.h"
///
#include "core/dynamic_variable/common_dvars.h"
#include "servers/display_server.h"

namespace vct {

SceneManager* g_scene_manager = new SceneManager;

Scene& SceneManager::get_scene() {
    assert(singleton().m_scene);
    return *singleton().m_scene;
}

// @TODO: move this to loader thread
void SceneManager::request_scene(std::string_view path) {
    std::thread t([](std::string_view scene_path) {
        Scene* new_scene = new Scene;
        SceneLoader loader(*new_scene);

        loader.LoadGLTF(scene_path);

        LOG("Scene '{}' loaded", scene_path);

        for (const MaterialComponent& material : new_scene->get_component_array<MaterialComponent>()) {
            for (int i = 0; i < array_length(material.mTextures); ++i) {
                const std::string& image_path = material.mTextures[i].name;
                if (!image_path.empty()) {
                    AssetManager::singleton().request_image_sync(image_path);
                }
            }
        }

        // @TODO:
        SceneManager::singleton().set_loading_scene(new_scene);
    },
                  path);

    t.detach();
}

void SceneManager::on_scene_changed(Scene* new_scene) {
    const int voxelTextureSize = DVAR_GET_INT(r_voxelSize);
    DEV_ASSERT(is_power_of_two(voxelTextureSize));
    DEV_ASSERT(voxelTextureSize <= 256);

    Camera& camera = gCamera;

    const vec4 cascades = DVAR_GET_VEC4(cam_cascades);

    camera.fovy = glm::radians(DVAR_GET_FLOAT(cam_fov));
    camera.zNear = cascades[0];
    camera.zFar = cascades[3];

    camera.yaw = glm::radians(180.0f);
    camera.pitch = 0.0f;
    camera.position = DVAR_GET_VEC3(cam_pos);

    const vec3 center = new_scene->m_bound.center();
    const vec3 size = new_scene->m_bound.size();
    const float worldSize = glm::max(size.x, glm::max(size.y, size.z));
    const float texelSize = 1.0f / static_cast<float>(voxelTextureSize);
    const float voxelSize = worldSize * texelSize;

    g_perFrameCache.cache.WorldCenter = center;
    g_perFrameCache.cache.WorldSizeHalf = 0.5f * worldSize;
    g_perFrameCache.cache.TexelSize = texelSize;
    g_perFrameCache.cache.VoxelSize = voxelSize;
}

// @TODO: fix
static mat4 R_HackLightSpaceMatrix(const vec3& lightDir) {
    const Scene& scene = SceneManager::get_scene();
    const vec3 center = scene.m_bound.center();
    const vec3 extents = scene.m_bound.size();
    const float size = 0.5f * glm::max(extents.x, glm::max(extents.y, extents.z));
    const mat4 V = glm::lookAt(center + glm::normalize(lightDir) * size, center, vec3(0, 1, 0));
    const mat4 P = glm::ortho(-size, size, -size, size, 0.0f, 2.0f * size);
    return P * V;
}

static void R_LightSpaceMatrix(const Camera& camera, const vec3& lightDir, mat4 lightPVs[NUM_CASCADES]) {
    unused(camera);
    lightPVs[0] = lightPVs[1] = lightPVs[2] = R_HackLightSpaceMatrix(lightDir);
    return;
}

static void Com_UpdateWorld() {
    Scene& scene = SceneManager::get_scene();

    // update camera
    auto [frameW, frameH] = DisplayServer::singleton().get_frame_size();
    const float aspect = (float)frameW / frameH;
    DEV_ASSERT(aspect > 0.0f);

    Camera& camera = gCamera;
    camera.SetAspect(aspect);
    camera.UpdatePV();

    DEV_ASSERT(scene.get_count<LightComponent>());
    const LightComponent& light_component = scene.get_component_array<LightComponent>()[0];
    auto light_entity = scene.get_entity<LightComponent>(0);
    const TransformComponent* light_transform = scene.get_component<TransformComponent>(light_entity);
    DEV_ASSERT(light_transform);

    vec3 light_dir = light_transform->GetLocalMatrix() * vec4(0, 1, 0, 0);

    // update lightspace matrices
    mat4 lightPVs[NUM_CASCADES];
    R_LightSpaceMatrix(camera, light_dir, lightPVs);

    for (size_t idx = 0; idx < vct::array_length(lightPVs); ++idx) {
        g_perFrameCache.cache.LightPVs[idx] = lightPVs[idx];
    }

    // update constants
    g_perFrameCache.cache.SunDir = light_dir;
    g_perFrameCache.cache.LightColor = light_component.color * light_component.energy;
    g_perFrameCache.cache.CamPos = camera.position;
    g_perFrameCache.cache.View = camera.View();
    g_perFrameCache.cache.Proj = camera.Proj();
    g_perFrameCache.cache.PV = camera.ProjView();
    g_perFrameCache.cache.CascadedClipZ = DVAR_GET_VEC4(cam_cascades);
    g_perFrameCache.cache.EnableGI = DVAR_GET_BOOL(r_enableVXGI);
    g_perFrameCache.cache.DebugCSM = DVAR_GET_BOOL(r_debugCSM);
    g_perFrameCache.cache.DebugTexture = DVAR_GET_INT(r_debugTexture);
    g_perFrameCache.cache.NoTexture = DVAR_GET_BOOL(r_noTexture);
    g_perFrameCache.cache.ScreenWidth = frameW;
    g_perFrameCache.cache.ScreenHeight = frameH;

    // SSAO
    g_perFrameCache.cache.SSAOKernelSize = DVAR_GET_INT(r_ssaoKernelSize);
    g_perFrameCache.cache.SSAOKernelRadius = DVAR_GET_FLOAT(r_ssaoKernelRadius);
    g_perFrameCache.cache.SSAONoiseSize = DVAR_GET_INT(r_ssaoNoiseSize);
    g_perFrameCache.cache.EnableSSAO = DVAR_GET_BOOL(r_enableSsao);

    // FXAA
    g_perFrameCache.cache.EnableFXAA = DVAR_GET_BOOL(r_enableFXAA);
}

bool SceneManager::initialize() {
    m_revision = 1;

    // make a simple scene
    {
        m_scene = new Scene;
        auto root = m_scene->create_transform_entity("world");
        m_scene->m_root = root;
        auto light = m_scene->create_omnilight_entity("omni light", vec3(1), 20.f);
        m_scene->attach_component(light, root);
        auto transform = m_scene->get_component<TransformComponent>(light);
        DEV_ASSERT(transform);
        mat4 rx = glm::rotate(glm::radians(10.0f), glm::vec3(1, 0, 0));
        mat4 rz = glm::rotate(glm::radians(10.0f), glm::vec3(0, 0, 1));
        transform->SetLocalTransform(rx * rz);
    }

    std::string_view scene_path = DVAR_GET_STRING(scene);

    if (!scene_path.empty()) {
        request_scene(scene_path);
    }

    return true;
}

void SceneManager::finalize() {}

void SceneManager::update(float dt) {
    Scene* new_scene = m_loading_scene.load();
    if (new_scene) {
        m_scene->merge(*new_scene);
        delete new_scene;
        m_loading_scene.store(nullptr);
        // @TODO: bump revision

        on_scene_changed(m_scene);
        ++m_revision;
    }

    Com_UpdateWorld();
    SceneManager::get_scene().update(dt);
}

}  // namespace vct
