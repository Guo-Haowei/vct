layout(location = 0) out vec4 out_position_metallic;
layout(location = 1) out vec4 out_normal_roughness;
layout(location = 2) out vec4 out_albedo;

in struct PS_INPUT {
    vec3 position;
    vec2 uv;
    mat3 TBN;
} ps_in;

#include "cbuffer.glsl"

void main() {
    vec4 albedo = AlbedoColor;

    if (HasAlbedoMap != 0) {
        albedo = texture(AlbedoMaps[TextureMapIdx], ps_in.uv);
    }
    if (albedo.a < 0.001) {
        discard;
    }

    float metallic = Metallic;
    float roughness = Roughness;
    if (HasPbrMap != 0) {
        // g roughness, b metallic
        vec3 mr = texture(PbrMaps[TextureMapIdx], ps_in.uv).rgb;
        metallic = mr.b;
        roughness = mr.g;
    }

    out_position_metallic.xyz = ps_in.position;
    out_position_metallic.w = metallic;

    // TODO: get rid of branching
    vec3 N;
    if (HasNormalMap != 0) {
        N = normalize(ps_in.TBN * (2.0 * texture(NormalMaps[TextureMapIdx], ps_in.uv).xyz - 1.0));
    } else {
        N = ps_in.TBN[2];
    }

    out_normal_roughness.xyz = N;
    out_normal_roughness.w = roughness;

    out_albedo.rgb = albedo.rgb;
}
