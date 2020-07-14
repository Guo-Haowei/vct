#version 450 core
#extension GL_NV_gpu_shader5: enable
#extension GL_NV_shader_atomic_float: enable
#extension GL_NV_shader_atomic_fp16_vector: enable

#define PI 3.14159265359

in vec3 pass_position;
in vec3 pass_normal;
in vec2 pass_uv;
in vec4 pass_light_space_position;

layout (rgba16f, binding = 0) uniform image3D u_albedo_texture;
layout (rgba16f, binding = 1) uniform image3D u_normal_texture;

layout (std140, binding = 1) uniform FSPerFrame
{
    vec3 light_position; // direction
    float _per_frame_pad0;
    vec3 light_color;
    float _per_frame_pad1;
    vec3 camera_position;
    float _per_frame_pad2;
};

layout (std140, binding = 2) uniform Material
{
    vec4 albedo_color;
    float metallic;
    float roughness;
    float has_metallic_roughness_texture;
    float has_normal_texture;
};

layout (std140, binding = 3) uniform Constant
{
    vec3 world_center;
    float world_size_half;
    float texel_size;
};

uniform sampler2D u_shadow_map;
uniform sampler2D u_albedo_map;
uniform sampler2D u_metallic_roughness_map;

float distributionGGX(float NdotH, float roughness);
float geometrySchlickGGX(float NdotV, float roughness);
float geometrySmith(float NdotV, float NdotL, float roughness);
vec3 fresnelSchlick(float cosTheta, const in vec3 F0);
float inShadow(const in vec4 position_light, float NdotL);

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

    vec3 world_position = pass_position;
    float metallic = metallic_roughness.r;
    float roughness = metallic_roughness.g;

    vec3 F0 = mix(vec3(0.04), albedo.rgb, metallic);
    vec3 Lo = vec3(0.0);

    vec3 N = normalize(pass_normal);
    vec3 L = normalize(light_position - world_position);
    vec3 V = normalize(camera_position - world_position);
    vec3 H = normalize(V + L);

    vec3 radiance = light_color;

    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    // cook-torrance brdf
    float NDF = distributionGGX(NdotH, roughness);
    float G = geometrySmith(NdotV, NdotL, roughness);
    vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 nom = NDF * G * F;
    float denom = 4 * NdotV * NdotL;

    vec3 specular = nom / max(denom, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL;

    float shadow = inShadow(pass_light_space_position, NdotL);

    float ambient = 0.15;

    vec3 color = (1.0 - shadow) * Lo + ambient * albedo.rgb;

    ///////////////////////////////////////////////////////////////////////////

    // write lighting information to texel
    vec3 voxel = (pass_position - world_center) / world_size_half; // normalize it to [-1, 1]
    voxel = 0.5 * voxel + vec3(0.5); // normalize to [0, 1]
    ivec3 dim = imageSize(u_albedo_texture);
    ivec3 coord = ivec3(dim * voxel);

    f16vec4 final_color = f16vec4(color.r, color.g, color.b, 1.0);
    // imageAtomicAdd(u_albedo_texture, coord, final_color);
    imageAtomicMax(u_albedo_texture, coord, final_color);

    // TODO: average normal
    f16vec4 normal_color = f16vec4(N.r, N.g, N.b, 1.0);
    imageAtomicAdd(u_normal_texture, coord, normal_color);
}

/// #include common.glsl
