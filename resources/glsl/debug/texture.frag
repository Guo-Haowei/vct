layout(location = 0) out vec4 out_color;
layout(location = 0) in vec2 pass_uv;

#include "cbuffer.glsl.h"

void main() {
    switch (c_debug_texture_id) {
        case 3: {
            out_color = vec4(texture(c_gbuffer_depth_map, pass_uv).rrr, 1.0);
            break;
        }
        case 4: {
            out_color = vec4(texture(c_gbuffer_albedo_map, pass_uv).rgb, 1.0);
            break;
        }
        case 5: {
            vec3 N = 0.5 * texture(c_gbuffer_normal_roughness_map, pass_uv).rgb + 0.5;
            out_color = vec4(N, 1.0);
            break;
        }
        case 6: {
            out_color = vec4(texture(c_gbuffer_position_metallic_map, pass_uv).aaa, 1.0);
            break;
        }
        case 7: {
            out_color = vec4(texture(c_gbuffer_normal_roughness_map, pass_uv).aaa, 1.0);
            break;
        }
        case 8: {
            out_color = vec4(texture(c_shadow_map, pass_uv).rrr, 1.0);
            break;
        }
        case 9: {
            out_color = vec4(texture(c_ssao_map, pass_uv).rrr, 1.0);
            break;
        }
        default:
            out_color = vec4(texture(c_fxaa_image, pass_uv).rgba);
            break;
    }
}
