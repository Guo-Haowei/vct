#version 450 core
layout (location = 0) out vec4 out_position_metallic;
layout (location = 1) out vec4 out_normal_roughness;
layout (location = 2) out vec4 out_albedo;

layout (location = 0) in vec3 pass_position;
layout (location = 1) in vec3 pass_normal;
layout (location = 2) in vec2 pass_uv;

layout (std140, binding = 2) uniform Material
{
    vec4 albedo_color;
    float metallic;
    float roughness;
    float has_metallic_roughness_texture;
    float has_normal_texture;
};

uniform sampler2D u_albedo_map;
uniform sampler2D u_metallic_roughness_map;

void main()
{
    vec4 albedo = mix(texture(u_albedo_map, pass_uv), albedo_color, albedo_color.a);
    if (albedo.a < 0.001)
        discard;

    // g roughness, b metallic
    vec2 metallic_roughness = mix(
        vec2(metallic, roughness),
        texture(u_metallic_roughness_map, pass_uv).bg,
        has_metallic_roughness_texture);

    out_position_metallic.xyz = pass_position;
    out_position_metallic.w = metallic_roughness.r;

    out_normal_roughness.xyz = normalize(pass_normal);
    out_normal_roughness.w = metallic_roughness.g;

    out_albedo.rgb = albedo.rgb;
}
