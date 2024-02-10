#pragma once
#include "render_graph/render_pass.h"

void create_passes();
void destroy_passes();

extern vct::rg::RenderPassGL g_shadow_pass;
extern vct::rg::RenderPassGL g_gbuffer_pass;

extern vct::rg::RenderPassGL g_ssao_pass;
extern vct::rg::RenderPassGL g_fxaa_pass;

extern vct::rg::RenderPassGL g_final_image_pass;
extern vct::rg::RenderPassGL g_viewer_pass;