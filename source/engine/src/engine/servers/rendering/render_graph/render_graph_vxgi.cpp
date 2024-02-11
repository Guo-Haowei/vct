#include "render_graph_vxgi.h"

#include "core/collections/rid_owner.h"
#include "core/math/frustum.h"
#include "servers/display_server.h"
#include "servers/rendering/rendering_dvars.h"

// @TODO: refactor
#include "../r_cbuffers.h"
#include "../shader_program_manager.h"
#include "scene/scene_manager.h"

// @TODO: refactor
using namespace vct;

/// textures
vct::RenderGraph g_render_graph;

uint32_t g_final_image;

extern GpuTexture g_albedoVoxel;
extern GpuTexture g_normalVoxel;

extern void FillMaterialCB(const MaterialData* mat, MaterialConstantBuffer& cb);

extern vct::RIDAllocator<MeshData> g_meshes;
extern vct::RIDAllocator<MaterialData> g_materials;

namespace vct {

void shadow_pass_func() {
    const vct::Scene& scene = SceneManager::get_scene();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glClear(GL_DEPTH_BUFFER_BIT);

    const int res = DVAR_GET_INT(r_shadow_res);
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

            const MeshData* drawData = g_meshes.get_or_null(mesh.gpu_resource);
            DEV_ASSERT(drawData);
            glBindVertexArray(drawData->vao);
            glDrawElements(GL_TRIANGLES, drawData->count, GL_UNSIGNED_INT, 0);
        }
    }

    glCullFace(GL_BACK);
    glUseProgram(0);
}

void voxelization_pass_func() {
    g_albedoVoxel.clear();
    g_normalVoxel.clear();

    const Scene& scene = SceneManager::get_scene();
    const int voxelSize = DVAR_GET_INT(r_voxel_size);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glViewport(0, 0, voxelSize, voxelSize);

    g_albedoVoxel.bindImageTexture(IMAGE_VOXEL_ALBEDO_SLOT);
    g_normalVoxel.bindImageTexture(IMAGE_VOXEL_NORMAL_SLOT);
    ShaderProgramManager::get(PROGRAM_VOXELIZATION).bind();

    const uint32_t numObjects = (uint32_t)scene.get_count<ObjectComponent>();
    for (uint32_t i = 0; i < numObjects; ++i) {
        const ObjectComponent& obj = scene.get_component_array<ObjectComponent>()[i];
        ecs::Entity entity = scene.get_entity<ObjectComponent>(i);
        DEV_ASSERT(scene.contains<TransformComponent>(entity));
        const TransformComponent& transform = *scene.get_component<TransformComponent>(entity);
        DEV_ASSERT(scene.contains<MeshComponent>(obj.meshID));
        const MeshComponent& mesh = *scene.get_component<MeshComponent>(obj.meshID);

        const mat4& M = transform.get_world_matrix();
        g_perBatchCache.cache.c_model_matrix = M;
        g_perBatchCache.cache.c_projection_view_model_matrix = g_perFrameCache.cache.c_projection_view_matrix * M;
        g_perBatchCache.Update();

        const MeshData* draw_data = g_meshes.get_or_null(mesh.gpu_resource);
        DEV_ASSERT(draw_data);
        glBindVertexArray(draw_data->vao);

        for (const auto& subset : mesh.subsets) {
            const MaterialComponent& material = *scene.get_component<MaterialComponent>(subset.material_id);
            const MaterialData* mat_data = g_materials.get_or_null(material.gpu_resource);
            DEV_ASSERT(mat_data);

            FillMaterialCB(mat_data, g_materialCache.cache);
            g_materialCache.Update();

            glDrawElements(GL_TRIANGLES, subset.index_count, GL_UNSIGNED_INT, (void*)(subset.index_offset * sizeof(uint32_t)));
        }
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // post process
    ShaderProgramManager::get(PROGRAM_VOXELIZATION_POST).bind();

    constexpr GLuint workGroupX = 512;
    constexpr GLuint workGroupY = 512;
    const GLuint workGroupZ = (voxelSize * voxelSize * voxelSize) / (workGroupX * workGroupY);

    glDispatchCompute(workGroupX, workGroupY, workGroupZ);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    g_albedoVoxel.bind();
    g_albedoVoxel.genMipMap();
    g_normalVoxel.bind();
    g_normalVoxel.genMipMap();
}

void gbuffer_pass_func() {
    auto [frameW, frameH] = DisplayServer::singleton().get_frame_size();
    glViewport(0, 0, frameW, frameH);

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

        const MeshData* drawData = g_meshes.get_or_null(mesh.gpu_resource);
        DEV_ASSERT(drawData);
        glBindVertexArray(drawData->vao);

        for (const auto& subset : mesh.subsets) {
            aabb = subset.local_bound;
            aabb.apply_matrix(M);
            if (!frustum.intersects(aabb)) {
                continue;
            }

            const MaterialComponent& material = *scene.get_component<MaterialComponent>(subset.material_id);
            const MaterialData* mat_data = g_materials.get_or_null(material.gpu_resource);
            DEV_ASSERT(mat_data);

            FillMaterialCB(mat_data, g_materialCache.cache);
            g_materialCache.Update();

            glDrawElements(GL_TRIANGLES, subset.index_count, GL_UNSIGNED_INT,
                           (void*)(subset.index_offset * sizeof(uint32_t)));
        }
    }

    glUseProgram(0);
}

void ssao_pass_func() {
    auto [frameW, frameH] = DisplayServer::singleton().get_frame_size();
    glViewport(0, 0, frameW, frameH);

    const auto& shader = ShaderProgramManager::get(PROGRAM_SSAO);

    glClear(GL_COLOR_BUFFER_BIT);

    shader.bind();

    R_DrawQuad();

    shader.unbind();
}

void lighting_pass_func() {
    auto [frameW, frameH] = DisplayServer::singleton().get_frame_size();
    glViewport(0, 0, frameW, frameH);

    const auto& program = ShaderProgramManager::get(PROGRAM_DEFERERD_VOXEL_LIGHTING);
    glClear(GL_COLOR_BUFFER_BIT);

    program.bind();

    R_DrawQuad();

    program.unbind();
}

void fxaa_pass_func() {
    auto [frameW, frameH] = DisplayServer::singleton().get_frame_size();
    glViewport(0, 0, frameW, frameH);

    const auto& program = ShaderProgramManager::get(PROGRAM_FXAA);

    glClear(GL_COLOR_BUFFER_BIT);

    program.bind();
    R_DrawQuad();
    program.unbind();
}

void final_pass_func() {
    auto [frameW, frameH] = DisplayServer::singleton().get_frame_size();
    glClearColor(.1f, .1f, .1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const auto& program = ShaderProgramManager::get(PROGRAM_FINAL_IMAGE);

    program.bind();
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, frameW, frameH);

    R_DrawQuad();

    program.unbind();
}

void create_render_graph_vxgi(RenderGraph& graph) {
    auto [w, h] = DisplayServer::singleton().get_frame_size();

    const int res = DVAR_GET_INT(r_shadow_res);
    DEV_ASSERT(math::is_power_of_two(res));

    // @TODO: split resource
    {  // shadow pass
        RenderPassDesc desc;
        desc.name = SHADOW_PASS_NAME;
        desc.depth_attachment = RenderTargetDesc{ SHADOW_PASS_OUTPUT, FORMAT_D32_FLOAT };
        desc.func = shadow_pass_func;
        desc.width = res;
        desc.height = res;

        graph.add_pass(desc);
    }
    {  // gbuffer pass
        RenderPassDesc desc;
        desc.name = GBUFFER_PASS_NAME;
        desc.dependencies = {};
        desc.color_attachments = {
            RenderTargetDesc{ GBUFFER_PASS_OUTPUT_POSITION, FORMAT_R16G16B16A16_FLOAT },
            RenderTargetDesc{ GBUFFER_PASS_OUTPUT_NORMAL, FORMAT_R16G16B16A16_FLOAT },
            RenderTargetDesc{ GBUFFER_PASS_OUTPUT_ALBEDO, FORMAT_R8G8B8A8_UINT },
        };
        desc.depth_attachment = RenderTargetDesc{ GBUFFER_PASS_OUTPUT_DEPTH, FORMAT_D32_FLOAT };
        desc.func = gbuffer_pass_func;
        desc.width = w;
        desc.height = h;

        graph.add_pass(desc);
    }
    {  // voxel pass
        RenderPassDesc desc;
        desc.type = RENDER_PASS_COMPUTE;
        desc.name = VOXELIZATION_PASS_NAME;
        desc.dependencies = { SHADOW_PASS_NAME };
        desc.func = voxelization_pass_func;

        graph.add_pass(desc);
    }
    {  // ssao pass
        RenderPassDesc desc;
        desc.name = SSAO_PASS_NAME;
        desc.dependencies = { GBUFFER_PASS_NAME };
        desc.color_attachments = { RenderTargetDesc{ SSAO_PASS_OUTPUT, FORMAT_R32_FLOAT } };
        desc.func = ssao_pass_func;
        desc.width = w;
        desc.height = h;

        graph.add_pass(desc);
    }
    {  // lighting pass
        RenderPassDesc desc;
        desc.name = LIGHTING_PASS_NAME;
        desc.dependencies = { GBUFFER_PASS_NAME, SHADOW_PASS_NAME, SSAO_PASS_NAME, VOXELIZATION_PASS_NAME };
        desc.color_attachments = { RenderTargetDesc{ LIGHTING_PASS_OUTPUT, FORMAT_R8G8B8A8_UINT } };
        desc.func = lighting_pass_func;
        desc.width = w;
        desc.height = h;

        graph.add_pass(desc);
    }
    {  // fxaa pass
        RenderPassDesc desc;
        desc.name = FXAA_PASS_NAME;
        desc.dependencies = { LIGHTING_PASS_NAME };
        desc.color_attachments = { RenderTargetDesc{ FXAA_PASS_OUTPUT, FORMAT_R8G8B8A8_UINT } };
        desc.func = fxaa_pass_func;
        desc.width = w;
        desc.height = h;

        graph.add_pass(desc);
    }
    {  // vier pass(final pass)
        RenderPassDesc desc;
        desc.name = FINAL_PASS_NAME;
        desc.dependencies = { FXAA_PASS_NAME };
        desc.color_attachments = { RenderTargetDesc{ "viewer_map", FORMAT_R8G8B8A8_UINT } };
        desc.width = w;
        desc.height = h;
        desc.func = final_pass_func;

        graph.add_pass(desc);
    }

    // @TODO: allow recompile
    graph.compile();
}

void create_render_graph_vxgi_debug(RenderGraph& graph) {
    auto [w, h] = DisplayServer::singleton().get_frame_size();

    const int res = DVAR_GET_INT(r_shadow_res);
    DEV_ASSERT(math::is_power_of_two(res));

    // @TODO: split resource
    {  // shadow pass
        RenderPassDesc desc;
        desc.name = SHADOW_PASS_NAME;
        desc.depth_attachment = RenderTargetDesc{ SHADOW_PASS_OUTPUT, FORMAT_D32_FLOAT };
        desc.func = shadow_pass_func;
        desc.width = res;
        desc.height = res;

        graph.add_pass(desc);
    }
    {  // voxel pass
        RenderPassDesc desc;
        desc.type = RENDER_PASS_COMPUTE;
        desc.name = VOXELIZATION_PASS_NAME;
        desc.dependencies = { SHADOW_PASS_NAME };
        desc.func = voxelization_pass_func;

        graph.add_pass(desc);
    }
    {  // voxel visualization pass
        RenderPassDesc desc;
        desc.name = VOXEL_VISUALIZATION_PASS_NAME;
        desc.dependencies = { VOXELIZATION_PASS_NAME };
        desc.color_attachments = {
            RenderTargetDesc{ VOXEL_VISUALIZATION_OUTPUT_COLOR, FORMAT_R8G8B8A8_UINT },
        };
        desc.depth_attachment = RenderTargetDesc{ VOXEL_VISUALIZATION_OUTPUT_DEPTH, FORMAT_D32_FLOAT };
        desc.func = gbuffer_pass_func;
        desc.width = w;
        desc.height = h;

        graph.add_pass(desc);
    }
    {  // vier pass(final pass)
        RenderPassDesc desc;
        desc.name = FINAL_PASS_NAME;
        desc.dependencies = { VOXEL_VISUALIZATION_PASS_NAME };
        desc.color_attachments = { RenderTargetDesc{ "viewer_map", FORMAT_R8G8B8A8_UINT } };
        desc.width = w;
        desc.height = h;
        desc.func = final_pass_func;

        graph.add_pass(desc);
    }

    // @TODO: allow recompile
    graph.compile();
}

}  // namespace vct
