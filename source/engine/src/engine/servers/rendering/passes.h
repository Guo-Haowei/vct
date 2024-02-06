#pragma once
#include "render_graph/render_pass.h"

void create_passes();
void destroy_passes();

extern vct::rg::RenderPassGL g_shadow_rt;
extern vct::rg::RenderPassGL g_gbuffer_rt;

extern vct::rg::RenderPassGL g_ssao_rt;
extern vct::rg::RenderPassGL g_fxaa_rt;

extern vct::rg::RenderPassGL g_final_image_rt;
extern vct::rg::RenderPassGL g_viewer_rt;