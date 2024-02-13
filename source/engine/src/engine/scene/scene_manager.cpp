#include "scene_manager.h"

#include "assets/asset_loader.h"
#include "imgui/imgui.h"
#include "servers/rendering/r_cbuffers.h"
///
#include "core/dynamic_variable/common_dvars.h"
#include "servers/display_server.h"
#include "servers/rendering/rendering_dvars.h"

namespace vct {

using ecs::Entity;

SceneManager* g_scene_manager = new SceneManager;

static bool deserialize_scene(Scene* scene, const std::string& path) {
    // @TODO:
    unused(scene);
    unused(path);
    Archive archive;
    if (!archive.open_read(path)) {
        return false;
    }

    return scene->serialize(archive);
}

static void create_empty_scene(Scene* scene) {
    Entity::set_seed(Entity::INVALID_ID + 1);

    auto root = scene->create_transform_entity("world");
    scene->m_root = root;
    {
        auto light = scene->create_omnilight_entity("omni light", vec3(1), 20.f);
        auto transform = scene->get_component<TransformComponent>(light);
        DEV_ASSERT(transform);
        mat4 r = glm::rotate(glm::radians(30.0f), glm::vec3(0, 0, 1));
        transform->set_local_transform(r);

        scene->attach_component(light, root);
    }

    {
        auto camera = scene->create_camera_entity("camera", 800.0f, 600.0f);
        scene->attach_component(camera);
    }
}

bool SceneManager::initialize() {
    m_revision = 1;

    // create an empty scene
    m_scene = new Scene;
    const std::string& path = DVAR_GET_STRING(project);
    if (path.empty()) {
        create_empty_scene(m_scene);
    } else {
        deserialize_scene(m_scene, path);
    }
    on_scene_changed(m_scene);

    return true;
}

void SceneManager::finalize() {}

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

void SceneManager::update(float dt) {
    Scene* new_scene = m_loading_scene.load();
    if (new_scene) {
        m_scene->merge(*new_scene);
        delete new_scene;
        m_loading_scene.store(nullptr);
        on_scene_changed(m_scene);
        ++m_revision;
    }

    Scene& scene = SceneManager::get_scene();

    auto [frameW, frameH] = DisplayServer::singleton().get_frame_size();

    // @TODO: refactor this
    CameraComponent& camera = scene.get_main_camera();
    if (frameW > 0 && frameH > 0) {
        camera.set_dimension((float)frameW, (float)frameH);
    }

    scene.update(dt);

    DEV_ASSERT(scene.get_count<LightComponent>());
    const LightComponent& light_component = scene.get_component_array<LightComponent>()[0];
    auto light_entity = scene.get_entity<LightComponent>(0);
    const TransformComponent* light_transform = scene.get_component<TransformComponent>(light_entity);
    DEV_ASSERT(light_transform);

    vec3 light_dir = light_transform->get_local_matrix() * vec4(0, 1, 0, 0);

    // update lightspace matrices
    mat4 lightPV = R_HackLightSpaceMatrix(light_dir);
    g_perFrameCache.cache.c_light_matricies[0] = g_perFrameCache.cache.c_light_matricies[1] = g_perFrameCache.cache.c_light_matricies[2] = lightPV;

    // update constants
    g_perFrameCache.cache.c_sun_direction = light_dir;
    g_perFrameCache.cache.c_light_color = light_component.color * light_component.energy;

    g_perFrameCache.cache.c_camera_position = camera.get_eye();
    g_perFrameCache.cache.c_view_matrix = camera.get_view_matrix();
    g_perFrameCache.cache.c_projection_matrix = camera.get_projection_matrix();
    g_perFrameCache.cache.c_projection_view_matrix = camera.get_projection_view_matrix();

    g_perFrameCache.cache.c_enable_vxgi = DVAR_GET_BOOL(r_enable_vxgi);
    g_perFrameCache.cache.c_debug_texture_id = DVAR_GET_INT(r_debugTexture);
    g_perFrameCache.cache.c_no_texture = DVAR_GET_BOOL(r_no_texture);
    g_perFrameCache.cache.c_screen_width = frameW;
    g_perFrameCache.cache.c_screen_height = frameH;

    // SSAO
    g_perFrameCache.cache.c_ssao_kernel_size = DVAR_GET_INT(r_ssaoKernelSize);
    g_perFrameCache.cache.c_ssao_kernel_radius = DVAR_GET_FLOAT(r_ssaoKernelRadius);
    g_perFrameCache.cache.c_ssao_noise_size = DVAR_GET_INT(r_ssaoNoiseSize);
    g_perFrameCache.cache.c_enable_ssao = DVAR_GET_BOOL(r_enableSsao);

    // c_fxaa_image
    g_perFrameCache.cache.c_enable_fxaa = DVAR_GET_BOOL(r_enableFXAA);
}

void SceneManager::request_scene(std::string_view path, ImporterName importer) {
    asset_loader::load_scene_async(importer, std::string(path), [](void* scene) {
        DEV_ASSERT(scene);
        Scene* new_scene = static_cast<Scene*>(scene);
        SceneManager::singleton().set_loading_scene(new_scene);
    });
}

void SceneManager::on_scene_changed(Scene* new_scene) {
    const int voxelTextureSize = DVAR_GET_INT(r_voxel_size);
    DEV_ASSERT(math::is_power_of_two(voxelTextureSize));
    DEV_ASSERT(voxelTextureSize <= 256);

    const vec3 center = new_scene->m_bound.center();
    const vec3 size = new_scene->m_bound.size();
    const float worldSize = glm::max(size.x, glm::max(size.y, size.z));
    const float texelSize = 1.0f / static_cast<float>(voxelTextureSize);
    const float voxelSize = worldSize * texelSize;

    g_perFrameCache.cache.c_world_center = center;
    g_perFrameCache.cache.c_world_size_half = 0.5f * worldSize;
    g_perFrameCache.cache.c_texel_size = texelSize;
    g_perFrameCache.cache.c_voxel_size = voxelSize;
}

Scene& SceneManager::get_scene() {
    assert(singleton().m_scene);
    return *singleton().m_scene;
}

}  // namespace vct
