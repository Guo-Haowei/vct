#include "passes.h"

#include "core/collections/fixed_stack.h"
#include "core/collections/rid_owner.h"
#include "core/dynamic_variable/common_dvars.h"
#include "core/math/frustum.h"
#include "servers/display_server.h"

// @TODO: refactor
#include "r_cbuffers.h"
#include "scene/scene_manager.h"
#include "shader_program_manager.h"
using namespace vct;
using namespace vct::rg;

vct::rg::RenderPassGL g_shadow_pass;
vct::rg::RenderPassGL g_gbuffer_pass;

vct::rg::RenderPassGL g_ssao_pass;
vct::rg::RenderPassGL g_fxaa_pass;

vct::rg::RenderPassGL g_final_image_pass;
vct::rg::RenderPassGL g_viewer_pass;
static void gbuffer_pass_func();
static void shadow_pass_func();
static void ssao_pass_func();
static void deferred_vct_pass();
static void fxaa_pass();

extern vct::RIDAllocator<MeshData> g_gpu_mesh;

static RenderPassDesc s_shadow_pass_desc = {
    .name = "shadow_pass",
    .depth_attachment = RenderTargetDesc{ "depth_map", FORMAT_D32_FLOAT },
    .func = shadow_pass_func,
};

static RenderPassDesc s_gbuffer_pass_desc = {
    .name = "gbuffer_pass",
    .color_attachments = {
        RenderTargetDesc{ "position_map", FORMAT_R16G16B16A16_FLOAT },
        RenderTargetDesc{ "normal_map", FORMAT_R16G16B16A16_FLOAT },
        RenderTargetDesc{ "albedo_map", FORMAT_R8G8B8A8_UINT },
    },
    .depth_attachment = RenderTargetDesc{ "depth_map", FORMAT_D32_FLOAT },
    .func = gbuffer_pass_func,
};

static RenderPassDesc s_ssao_pass_desc = {
    .name = "ssao_pass",
    .color_attachments = {
        RenderTargetDesc{ "ssao_map", FORMAT_R32_FLOAT },
    },
    .func = ssao_pass_func,
};

static RenderPassDesc s_fxaa_pass_desc = {
    .name = "fxaa_pass",
    .color_attachments = {
        RenderTargetDesc{ "fxaa_map", FORMAT_R8G8B8A8_UINT },
    },
    .func = fxaa_pass,
};

static RenderPassDesc s_final_image_pass_desc = {
    .name = "final_image_pass",
    .color_attachments = {
        RenderTargetDesc{ "ssao_map", FORMAT_R8G8B8A8_UINT },
    },
    .func = deferred_vct_pass,
};

static RenderPassDesc s_viewer_pass_desc = {
    .name = "viewer_pass",
    .color_attachments = {
        RenderTargetDesc{ "viewer_map", FORMAT_R8G8B8A8_UINT },
    },
};

// void RenderTarget::Destroy() {
//     mDepthAttachment.destroy();
//     for (int i = 0; i < mColorAttachmentCount; ++i) {
//         mColorAttachments[i].destroy();
//     }
//
//     glDeleteFramebuffers(1, &m_handle);
//     m_handle = 0;
// }

uint32_t g_final_image;

void create_passes() {
    auto [w, h] = DisplayServer::singleton().get_frame_size();

    const int res = DVAR_GET_INT(r_shadowRes);
    DEV_ASSERT(math::is_power_of_two(res));

    g_ssao_pass.create(s_ssao_pass_desc, w, h);
    g_gbuffer_pass.create(s_gbuffer_pass_desc, w, h);
    g_shadow_pass.create(s_shadow_pass_desc, res, res);

    g_fxaa_pass.create(s_fxaa_pass_desc, w, h);
    g_final_image_pass.create(s_final_image_pass_desc, w, h);
    g_viewer_pass.create(s_viewer_pass_desc, w, h);

    g_final_image = g_viewer_pass.get_color_attachment(0);
}

void destroy_passes() {
}

extern void FillMaterialCB(const MaterialData* mat, MaterialConstantBuffer& cb);

static void shadow_pass_func() {
    const vct::Scene& scene = SceneManager::get_scene();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glClear(GL_DEPTH_BUFFER_BIT);

    const int res = DVAR_GET_INT(r_shadowRes);
    // render scene 3 times

    const uint32_t numObjects = (uint32_t)scene.get_count<ObjectComponent>();
    // @TODO: fix shadow cascade
    for (int idx = 0; idx < 1; ++idx) {
        glViewport(idx * res, 0, res, res);
        const mat4& PV = g_perFrameCache.cache.LightPVs[idx];
        const Frustum frustum(PV);

        for (uint32_t i = 0; i < numObjects; ++i) {
            const ObjectComponent& obj = scene.get_component_array<ObjectComponent>()[i];
            ecs::Entity entity = scene.get_entity<ObjectComponent>(i);
            DEV_ASSERT(scene.contains<TransformComponent>(entity));
            const TransformComponent& transform = *scene.get_component<TransformComponent>(entity);
            DEV_ASSERT(scene.contains<MeshComponent>(obj.meshID));
            const MeshComponent& mesh = *scene.get_component<MeshComponent>(obj.meshID);

            const mat4& M = transform.get_world_matrix();
            AABB aabb = mesh.local_bound;
            aabb.apply_matrix(M);
            if (!frustum.intersects(aabb)) {
                continue;
            }

            const bool has_bone = mesh.armature_id.is_valid();

            if (has_bone) {
                auto& armature = *scene.get_component<ArmatureComponent>(mesh.armature_id);
                DEV_ASSERT(armature.boneTransforms.size() <= MAX_BONE_NUMBER);

                memcpy(g_boneCache.cache.c_bones, armature.boneTransforms.data(), sizeof(mat4) * armature.boneTransforms.size());
                g_boneCache.Update();
            }

            const auto& program = ShaderProgramManager::get(has_bone ? PROGRAM_DPETH_ANIMATED : PROGRAM_DPETH_STATIC);
            program.bind();

            g_perBatchCache.cache.c_projection_view_model_matrix = PV * M;
            g_perBatchCache.cache.c_model_matrix = M;
            g_perBatchCache.Update();

            const MeshData* drawData = g_gpu_mesh.get_or_null(mesh.gpu_resource);
            DEV_ASSERT(drawData);
            glBindVertexArray(drawData->vao);
            glDrawElements(GL_TRIANGLES, drawData->count, GL_UNSIGNED_INT, 0);
        }
    }

    glCullFace(GL_BACK);
    glUseProgram(0);
}

static void gbuffer_pass_func() {
    vct::Scene& scene = SceneManager::get_scene();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CameraComponent& camera = scene.get_main_camera();

    Frustum frustum(camera.get_projection_view_matrix());

    // @TODO: sort animated / non-animated
    const uint32_t numObjects = (uint32_t)scene.get_count<ObjectComponent>();
    for (uint32_t i = 0; i < numObjects; ++i) {
        const ObjectComponent& obj = scene.get_component_array<ObjectComponent>()[i];
        ecs::Entity entity = scene.get_entity<ObjectComponent>(i);
        DEV_ASSERT(scene.contains<TransformComponent>(entity));
        const TransformComponent& transform = *scene.get_component<TransformComponent>(entity);
        DEV_ASSERT(scene.contains<MeshComponent>(obj.meshID));

        const MeshComponent& mesh = *scene.get_component<MeshComponent>(obj.meshID);

        const mat4& M = transform.get_world_matrix();
        AABB aabb = mesh.local_bound;
        aabb.apply_matrix(M);
        if (!frustum.intersects(aabb)) {
            continue;
        }

        bool has_bone = mesh.armature_id.is_valid();

        if (has_bone) {
            auto& armature = *scene.get_component<ArmatureComponent>(mesh.armature_id);
            DEV_ASSERT(armature.boneTransforms.size() <= MAX_BONE_NUMBER);

            memcpy(g_boneCache.cache.c_bones, armature.boneTransforms.data(), sizeof(mat4) * armature.boneTransforms.size());
            g_boneCache.Update();
        }

        const auto& program = ShaderProgramManager::get(has_bone ? PROGRAM_GBUFFER_ANIMATED : PROGRAM_GBUFFER_STATIC);
        program.bind();

        g_perBatchCache.cache.c_model_matrix = M;
        g_perBatchCache.cache.c_projection_view_model_matrix = g_perFrameCache.cache.c_projection_view_matrix * M;
        g_perBatchCache.Update();

        const MeshData* drawData = g_gpu_mesh.get_or_null(mesh.gpu_resource);
        DEV_ASSERT(drawData);
        glBindVertexArray(drawData->vao);

        for (const auto& subset : mesh.subsets) {
            aabb = subset.local_bound;
            aabb.apply_matrix(M);
            if (!frustum.intersects(aabb)) {
                continue;
            }

            const MaterialComponent& material = *scene.get_component<MaterialComponent>(subset.material_id);
            const MaterialData* matData = reinterpret_cast<MaterialData*>(material.gpuResource);

            FillMaterialCB(matData, g_materialCache.cache);
            g_materialCache.Update();

            glDrawElements(GL_TRIANGLES, subset.index_count, GL_UNSIGNED_INT,
                           (void*)(subset.index_offset * sizeof(uint32_t)));
        }
    }

    glUseProgram(0);
}

static void ssao_pass_func() {
    const auto& shader = ShaderProgramManager::get(ProgramType::SSAO);

    glClear(GL_COLOR_BUFFER_BIT);

    shader.bind();

    R_DrawQuad();

    shader.unbind();
}

static void deferred_vct_pass() {
    const auto& program = ShaderProgramManager::get(ProgramType::VCT_DEFERRED);
    glClear(GL_COLOR_BUFFER_BIT);

    program.bind();

    R_DrawQuad();

    program.unbind();
}

static void fxaa_pass() {
    const auto& program = ShaderProgramManager::get(ProgramType::FXAA);

    glClear(GL_COLOR_BUFFER_BIT);

    program.bind();
    R_DrawQuad();
    program.unbind();
}
