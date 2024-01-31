layout(location = 0) out vec4 out_color;
layout(location = 0) in vec2 pass_uv;

#include "cbuffer.glsl"

void main() {
    switch (DebugTexture) {
        case 3: {
            out_color = vec4(texture(GbufferDepthMap, pass_uv).rrr, 1.0);
            break;
        }
        case 4: {
            out_color = vec4(texture(GbufferAlbedoMap, pass_uv).rgb, 1.0);
            break;
        }
        case 5: {
            vec3 N = 0.5 * texture(GbufferNormalRoughnessMap, pass_uv).rgb + 0.5;
            out_color = vec4(N, 1.0);
            break;
        }
        case 6: {
            out_color = vec4(texture(GbufferPositionMetallicMap, pass_uv).aaa, 1.0);
            break;
        }
        case 7: {
            out_color = vec4(texture(GbufferNormalRoughnessMap, pass_uv).aaa, 1.0);
            break;
        }
        case 8: {
            out_color = vec4(texture(ShadowMap, pass_uv).rrr, 1.0);
            break;
        }
        case 9: {
            out_color = vec4(texture(SSAOMap, pass_uv).rrr, 1.0);
            break;
        }
        default:
            out_color = vec4(texture(FXAA, pass_uv).rgba);
            // out_color = vec4( texture( FinalImage, pass_uv ).rgba );
            break;
    }
}
