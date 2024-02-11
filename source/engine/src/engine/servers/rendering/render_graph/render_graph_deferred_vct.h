#pragma once
#include "render_graph.h"

#define SHADOW_PASS_NAME       "shadow_pass"
#define VOXELIZATION_PASS_NAME "voxelization_pass"
#define GBUFFER_PASS_NAME      "gbuffer_pass"
#define LIGHTING_PASS_NAME     "lighting_pass"
#define SSAO_PASS_NAME         "ssao_pass"
#define FXAA_PASS_NAME         "fxaa_pass"
#define FINAL_PASS_NAME        "final_pass"

#define SHADOW_PASS_OUTPUT           SHADOW_PASS_NAME "_output"
#define SSAO_PASS_OUTPUT             SSAO_PASS_NAME "_output"
#define FXAA_PASS_OUTPUT             FXAA_PASS_NAME "_output"
#define LIGHTING_PASS_OUTPUT         LIGHTING_PASS_NAME "_output"
#define GBUFFER_PASS_OUTPUT_POSITION "gbuffer_output_position"
#define GBUFFER_PASS_OUTPUT_NORMAL   "gbuffer_output_normal"
#define GBUFFER_PASS_OUTPUT_ALBEDO   "gbuffer_output_albedo"
#define GBUFFER_PASS_OUTPUT_DEPTH    "gbuffer_output_depth"

// @TODO: refactor
extern vct::RenderGraph g_render_graph;

namespace vct {

void shadow_pass_func();
void gbuffer_pass_func();
void voxelization_pass_func();
void ssao_pass_func();
void lighting_pass_func();
void fxaa_pass_func();
void final_pass_func();

void create_render_graph_deferred_vct(RenderGraph& graph);

}  // namespace vct
