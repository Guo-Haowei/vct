#pragma once
#include "render_graph/render_graph.h"

void create_passes();
void destroy_passes();

extern vct::RenderGraph g_render_graph;

extern std::shared_ptr<vct::RenderPass> g_shadow_pass;
extern std::shared_ptr<vct::RenderPass> g_gbuffer_pass;
extern std::shared_ptr<vct::RenderPass> g_voxelization_pass;

extern std::shared_ptr<vct::RenderPass> g_ssao_pass;
extern std::shared_ptr<vct::RenderPass> g_fxaa_pass;
extern std::shared_ptr<vct::RenderPass> g_lighting_pass;
extern std::shared_ptr<vct::RenderPass> g_viewer_pass;