#include "passes.h"

#include "core/collections/fixed_stack.h"
#include "core/dynamic_variable/common_dvars.h"
#include "core/math/frustum.h"
#include "servers/display_server.h"

// @TODO: refactor
#include "Core/camera.h"
#include "Framework/ProgramManager.h"
#include "Framework/SceneManager.h"
#include "r_cbuffers.h"
using namespace vct;
using namespace vct::rg;

vct::rg::RenderPassGL g_shadow_rt;
vct::rg::RenderPassGL g_gbuffer_rt;

vct::rg::RenderPassGL g_ssao_rt;
vct::rg::RenderPassGL g_fxaa_rt;

vct::rg::RenderPassGL g_final_image_rt;
vct::rg::RenderPassGL g_viewer_rt;

static void gbuffer_pass_func();
static void shadow_pass_func();
static void ssao_pass_func();
static void deferred_vct_pass();
static void fxaa_pass();

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
//     glDeleteFramebuffers(1, &mHandle);
//     mHandle = 0;
// }

uint32_t g_final_image;

void create_passes() {
    auto [w, h] = DisplayServer::singleton().get_frame_size();

    const int res = DVAR_GET_INT(r_shadowRes);
    DEV_ASSERT(is_power_of_two(res));

    g_ssao_rt.create(s_ssao_pass_desc, w, h);
    g_gbuffer_rt.create(s_gbuffer_pass_desc, w, h);
    g_shadow_rt.create(s_shadow_pass_desc, res, res);

    g_fxaa_rt.create(s_fxaa_pass_desc, w, h);
    g_final_image_rt.create(s_final_image_pass_desc, w, h);
    g_viewer_rt.create(s_viewer_pass_desc, w, h);

    g_final_image = g_viewer_rt.get_color_attachment(0);
}

void destroy_passes() {
}

extern void FillMaterialCB(const MaterialData* mat, MaterialCB& cb);

static void shadow_pass_func() {
    const Scene& scene = SceneManager::get_scene();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glClear(GL_DEPTH_BUFFER_BIT);
    const auto& program = gProgramManager->GetShaderProgram(ProgramType::SHADOW);
    program.Bind();

    const int res = DVAR_GET_INT(r_shadowRes);
    // render scene 3 times

    const uint32_t numObjects = (uint32_t)scene.GetCount<ObjectComponent>();
    for (int idx = 0; idx < NUM_CASCADES; ++idx) {
        glViewport(idx * res, 0, res, res);
        const mat4& PV = g_perFrameCache.cache.LightPVs[idx];
        const Frustum frustum(PV);

        for (uint32_t i = 0; i < numObjects; ++i) {
            const ObjectComponent& obj = scene.GetComponentArray<ObjectComponent>()[i];
            ecs::Entity entity = scene.GetEntity<ObjectComponent>(i);
            DEV_ASSERT(scene.Contains<TransformComponent>(entity));
            const TransformComponent& transform = *scene.GetComponent<TransformComponent>(entity);
            DEV_ASSERT(scene.Contains<MeshComponent>(obj.meshID));
            const MeshComponent& mesh = *scene.GetComponent<MeshComponent>(obj.meshID);

            const mat4& M = transform.GetWorldMatrix();
            AABB aabb = mesh.mLocalBound;
            aabb.apply_matrix(M);
            if (!frustum.intersects(aabb)) {
                continue;
            }

            g_perBatchCache.cache.PVM = PV * M;
            g_perBatchCache.cache.Model = M;
            g_perBatchCache.Update();

            const MeshData* drawData = reinterpret_cast<MeshData*>(mesh.gpuResource);
            glBindVertexArray(drawData->vao);
            glDrawElements(GL_TRIANGLES, drawData->count, GL_UNSIGNED_INT, 0);
        }
    }

    glCullFace(GL_BACK);
}

static void gbuffer_pass_func() {
    Scene& scene = SceneManager::get_scene();
    const auto& program = gProgramManager->GetShaderProgram(ProgramType::GBUFFER);

    program.Bind();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Frustum frustum(gCamera.ProjView());

    const uint32_t numObjects = (uint32_t)scene.GetCount<ObjectComponent>();
    for (uint32_t i = 0; i < numObjects; ++i) {
        const ObjectComponent& obj = scene.GetComponentArray<ObjectComponent>()[i];
        ecs::Entity entity = scene.GetEntity<ObjectComponent>(i);
        DEV_ASSERT(scene.Contains<TransformComponent>(entity));
        const TransformComponent& transform = *scene.GetComponent<TransformComponent>(entity);
        DEV_ASSERT(scene.Contains<MeshComponent>(obj.meshID));
        const MeshComponent& mesh = *scene.GetComponent<MeshComponent>(obj.meshID);

        const mat4& M = transform.GetWorldMatrix();
        AABB aabb = mesh.mLocalBound;
        aabb.apply_matrix(M);
        if (!frustum.intersects(aabb)) {
            continue;
        }

        g_perBatchCache.cache.Model = M;
        g_perBatchCache.cache.PVM = g_perFrameCache.cache.PV * M;
        g_perBatchCache.Update();

        const MeshData* drawData = reinterpret_cast<MeshData*>(mesh.gpuResource);
        glBindVertexArray(drawData->vao);

        for (const auto& subset : mesh.mSubsets) {
            aabb = subset.localBound;
            aabb.apply_matrix(M);
            if (!frustum.intersects(aabb)) {
                continue;
            }

            const MaterialComponent& material = *scene.GetComponent<MaterialComponent>(subset.materialID);
            const MaterialData* matData = reinterpret_cast<MaterialData*>(material.gpuResource);

            FillMaterialCB(matData, g_materialCache.cache);
            g_materialCache.Update();

            glDrawElements(GL_TRIANGLES, subset.indexCount, GL_UNSIGNED_INT,
                           (void*)(subset.indexOffset * sizeof(uint32_t)));
        }
    }

    program.Unbind();
}

static void ssao_pass_func() {
    const auto& shader = gProgramManager->GetShaderProgram(ProgramType::SSAO);

    glClear(GL_COLOR_BUFFER_BIT);

    shader.Bind();

    R_DrawQuad();

    shader.Unbind();
}

static void deferred_vct_pass() {
    const auto& program = gProgramManager->GetShaderProgram(ProgramType::VCT_DEFERRED);
    glClear(GL_COLOR_BUFFER_BIT);

    program.Bind();

    R_DrawQuad();

    program.Unbind();
}

static void fxaa_pass() {
    const auto& program = gProgramManager->GetShaderProgram(ProgramType::FXAA);

    glClear(GL_COLOR_BUFFER_BIT);

    program.Bind();
    R_DrawQuad();
    program.Unbind();
}
