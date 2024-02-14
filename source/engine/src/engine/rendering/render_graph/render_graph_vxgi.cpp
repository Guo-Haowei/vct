#include "render_graph_vxgi.h"

#include "core/base/rid_owner.h"
#include "core/math/frustum.h"
#include "rendering/rendering_dvars.h"
#include "servers/display_server.h"

// @TODO: refactor
#include "core/framework/graphics_manager.h"
#include "core/framework/scene_manager.h"
#include "rendering/r_cbuffers.h"
#include "rendering/render_data.h"
#include "rendering/shader_program_manager.h"

/// textures
vct::RenderGraph g_render_graph;

uint32_t g_final_image;

extern GpuTexture g_albedoVoxel;
extern GpuTexture g_normalVoxel;
extern MeshData g_box;

extern void FillMaterialCB(const MaterialData* mat, MaterialConstantBuffer& cb);

extern vct::RIDAllocator<MeshData> g_meshes;
extern vct::RIDAllocator<MaterialData> g_materials;

namespace vct {

// @TODO: refactor render passes
void shadow_pass_func() {
    const vct::Scene& scene = SceneManager::get_scene();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glClear(GL_DEPTH_BUFFER_BIT);

    const int res = DVAR_GET_INT(r_shadow_res);
    glViewport(0, 0, res, res);

    auto render_data = GraphicsManager::singleton().get_render_data();
    RenderData::Pass& pass = render_data->shadow_pass;

    for (const auto& draw : pass.draws) {
        const bool has_bone = draw.armature_id.is_valid();

        if (has_bone) {
            auto& armature = *scene.get_component<ArmatureComponent>(draw.armature_id);
            DEV_ASSERT(armature.bone_transforms.size() <= MAX_BONE_NUMBER);

            memcpy(g_boneCache.cache.c_bones, armature.bone_transforms.data(), sizeof(mat4) * armature.bone_transforms.size());
            g_boneCache.Update();
        }

        const auto& program = ShaderProgramManager::get(has_bone ? PROGRAM_DPETH_ANIMATED : PROGRAM_DPETH_STATIC);
        program.bind();

        g_perBatchCache.cache.c_projection_view_model_matrix = pass.projection_view_matrix * draw.world_matrix;
        g_perBatchCache.cache.c_model_matrix = draw.world_matrix;
        g_perBatchCache.Update();

        glBindVertexArray(draw.mesh_data->vao);
        glDrawElements(GL_TRIANGLES, draw.mesh_data->count, GL_UNSIGNED_INT, 0);
    }

    glCullFace(GL_BACK);
    glUseProgram(0);
}

void voxelization_pass_func() {
    g_albedoVoxel.clear();
    g_normalVoxel.clear();

    const int voxel_size = DVAR_GET_INT(r_voxel_size);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glViewport(0, 0, voxel_size, voxel_size);

    g_albedoVoxel.bindImageTexture(IMAGE_VOXEL_ALBEDO_SLOT);
    g_normalVoxel.bindImageTexture(IMAGE_VOXEL_NORMAL_SLOT);
    ShaderProgramManager::get(PROGRAM_VOXELIZATION).bind();

    auto render_data = GraphicsManager::singleton().get_render_data();
    RenderData::Pass& pass = render_data->main_pass;

    for (const auto& draw : pass.draws) {
        const bool has_bone = draw.armature_id.is_valid();

        if (has_bone) {
            auto& armature = *render_data->scene->get_component<ArmatureComponent>(draw.armature_id);
            DEV_ASSERT(armature.bone_transforms.size() <= MAX_BONE_NUMBER);

            memcpy(g_boneCache.cache.c_bones, armature.bone_transforms.data(), sizeof(mat4) * armature.bone_transforms.size());
            g_boneCache.Update();
        }

        g_perBatchCache.cache.c_projection_view_model_matrix = pass.projection_view_matrix * draw.world_matrix;
        g_perBatchCache.cache.c_model_matrix = draw.world_matrix;
        g_perBatchCache.Update();

        glBindVertexArray(draw.mesh_data->vao);

        for (const auto& subset : draw.subsets) {
            FillMaterialCB(subset.material_data, g_materialCache.cache);
            g_materialCache.Update();

            glDrawElements(GL_TRIANGLES, subset.index_count, GL_UNSIGNED_INT, (void*)(subset.index_offset * sizeof(uint32_t)));
        }
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // post process
    ShaderProgramManager::get(PROGRAM_VOXELIZATION_POST).bind();

    constexpr GLuint workGroupX = 512;
    constexpr GLuint workGroupY = 512;
    const GLuint workGroupZ = (voxel_size * voxel_size * voxel_size) / (workGroupX * workGroupY);

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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto render_data = GraphicsManager::singleton().get_render_data();
    RenderData::Pass& pass = render_data->main_pass;

    for (const auto& draw : pass.draws) {
        const bool has_bone = draw.armature_id.is_valid();

        if (has_bone) {
            auto& armature = *render_data->scene->get_component<ArmatureComponent>(draw.armature_id);
            DEV_ASSERT(armature.bone_transforms.size() <= MAX_BONE_NUMBER);

            memcpy(g_boneCache.cache.c_bones, armature.bone_transforms.data(), sizeof(mat4) * armature.bone_transforms.size());
            g_boneCache.Update();
        }

        const auto& program = ShaderProgramManager::get(has_bone ? PROGRAM_GBUFFER_ANIMATED : PROGRAM_GBUFFER_STATIC);
        program.bind();

        g_perBatchCache.cache.c_projection_view_model_matrix = pass.projection_view_matrix * draw.world_matrix;
        g_perBatchCache.cache.c_model_matrix = draw.world_matrix;
        g_perBatchCache.Update();

        glBindVertexArray(draw.mesh_data->vao);

        for (const auto& subset : draw.subsets) {
            FillMaterialCB(subset.material_data, g_materialCache.cache);
            g_materialCache.Update();

            glDrawElements(GL_TRIANGLES, subset.index_count, GL_UNSIGNED_INT, (void*)(subset.index_offset * sizeof(uint32_t)));
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
    glClear(GL_COLOR_BUFFER_BIT);

    const auto& program = ShaderProgramManager::get(PROGRAM_LIGHTING_VXGI);
    program.bind();
    R_DrawQuad();
    program.unbind();
}

void fxaa_pass_func() {
    auto [frameW, frameH] = DisplayServer::singleton().get_frame_size();
    glViewport(0, 0, frameW, frameH);
    glClear(GL_COLOR_BUFFER_BIT);

    const auto& program = ShaderProgramManager::get(PROGRAM_FXAA);
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

void debug_vxgi_pass_func() {
    auto [width, height] = vct::DisplayServer::singleton().get_frame_size();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    // glDisable(GL_CULL_FACE);
    // glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    const auto& program = vct::ShaderProgramManager::get(vct::PROGRAM_DEBUG_VOXEL);
    program.bind();

    glBindVertexArray(g_box.vao);

    const int size = DVAR_GET_INT(r_voxel_size);
    glDrawElementsInstanced(GL_TRIANGLES, g_box.count, GL_UNSIGNED_INT, 0, size * size * size);

    program.unbind();
}

void create_render_graph_vxgi(RenderGraph& graph) {
    auto [w, h] = DisplayServer::singleton().get_frame_size();

    const int res = DVAR_GET_INT(r_shadow_res);
    DEV_ASSERT(math::is_power_of_two(res));

    // @TODO: split resource
    {  // shadow pass
        RenderPassDesc desc;
        desc.name = SHADOW_PASS;
        desc.depth_attachment = RenderTargetDesc{ SHADOW_PASS_OUTPUT, FORMAT_D32_FLOAT };
        desc.func = shadow_pass_func;
        desc.width = res;
        desc.height = res;

        graph.add_pass(desc);
    }
    {  // gbuffer pass
        RenderPassDesc desc;
        desc.name = GBUFFER_PASS;
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
        desc.name = VOXELIZATION_PASS;
        desc.dependencies = { SHADOW_PASS };
        desc.func = voxelization_pass_func;

        graph.add_pass(desc);
    }
    {  // ssao pass
        RenderPassDesc desc;
        desc.name = SSAO_PASS;
        desc.dependencies = { GBUFFER_PASS };
        desc.color_attachments = { RenderTargetDesc{ SSAO_PASS_OUTPUT, FORMAT_R32_FLOAT } };
        desc.func = ssao_pass_func;
        desc.width = w;
        desc.height = h;

        graph.add_pass(desc);
    }
    {  // lighting pass
        RenderPassDesc desc;
        desc.name = LIGHTING_PASS;
        desc.dependencies = { GBUFFER_PASS, SHADOW_PASS, SSAO_PASS, VOXELIZATION_PASS };
        desc.color_attachments = { RenderTargetDesc{ LIGHTING_PASS_OUTPUT, FORMAT_R8G8B8A8_UINT } };
        desc.func = lighting_pass_func;
        desc.width = w;
        desc.height = h;

        graph.add_pass(desc);
    }
    {  // fxaa pass
        RenderPassDesc desc;
        desc.name = FXAA_PASS;
        desc.dependencies = { LIGHTING_PASS };
        desc.color_attachments = { RenderTargetDesc{ FXAA_PASS_OUTPUT, FORMAT_R8G8B8A8_UINT } };
        desc.func = fxaa_pass_func;
        desc.width = w;
        desc.height = h;

        graph.add_pass(desc);
    }
    {  // vier pass(final pass)
        RenderPassDesc desc;
        desc.name = FINAL_PASS;
        desc.dependencies = { FXAA_PASS };
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
    // @TODO: split resource
    auto [w, h] = DisplayServer::singleton().get_frame_size();

    const int res = DVAR_GET_INT(r_shadow_res);
    DEV_ASSERT(math::is_power_of_two(res));

    {  // shadow pass
        RenderPassDesc desc;
        desc.name = SHADOW_PASS;
        desc.depth_attachment = RenderTargetDesc{ SHADOW_PASS_OUTPUT, FORMAT_D32_FLOAT };
        desc.func = shadow_pass_func;
        desc.width = res;
        desc.height = res;

        graph.add_pass(desc);
    }
    {  // voxel pass
        RenderPassDesc desc;
        desc.type = RENDER_PASS_COMPUTE;
        desc.name = VOXELIZATION_PASS;
        desc.dependencies = { SHADOW_PASS };
        desc.func = voxelization_pass_func;

        graph.add_pass(desc);
    }
    {  // vxgi debug pass
        RenderPassDesc desc;
        desc.name = VXGI_DEBUG_PASS;
        desc.dependencies = { SHADOW_PASS, VOXELIZATION_PASS };
        desc.color_attachments = { RenderTargetDesc{ LIGHTING_PASS_OUTPUT, FORMAT_R8G8B8A8_UINT } };
        desc.depth_attachment = { RenderTargetDesc{ "depth", FORMAT_D32_FLOAT } };
        desc.func = debug_vxgi_pass_func;
        desc.width = w;
        desc.height = h;

        graph.add_pass(desc);
    }
    {  // fxaa pass
        RenderPassDesc desc;
        desc.name = FXAA_PASS;
        desc.dependencies = { VXGI_DEBUG_PASS };
        desc.color_attachments = { RenderTargetDesc{ FXAA_PASS_OUTPUT, FORMAT_R8G8B8A8_UINT } };
        desc.func = fxaa_pass_func;
        desc.width = w;
        desc.height = h;

        graph.add_pass(desc);
    }

    // @TODO: allow recompile
    graph.compile();
}

}  // namespace vct
