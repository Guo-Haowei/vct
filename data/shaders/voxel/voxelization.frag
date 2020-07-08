#version 450 core
#extension GL_NV_gpu_shader5: enable
#extension GL_NV_shader_atomic_float: enable
#extension GL_NV_shader_atomic_fp16_vector: enable

in vec3 pass_position_world;
in vec4 pass_position_light;
in vec3 pass_normal;
in vec2 pass_uv;

layout (rgba16f, binding = 0) uniform image3D u_albedo_texture;
layout (rgba16f, binding = 1) uniform image3D u_normal_texture;

uniform vec4 u_world; // xyz : world center; w : world size

layout (std140, binding = 1) uniform Light
{
    vec3 light_position;
    float padding;
    mat4 light_space;
};

layout (std140, binding = 2) uniform Material
{
    vec4 albedo_color;
};

uniform sampler2D u_albedo_map;
uniform sampler2D u_shadow_map;

float in_shadow(vec4 position_light)
{
    vec3 coords = position_light.xyz / position_light.w;
    coords = 0.5 * coords + 0.5;
    float closest_depth = texture(u_shadow_map, coords.xy).r;
    float current_depth = coords.z;

    float bias = 0.005;
    return current_depth - bias > closest_depth ? 1.0 : 0.0;
}

void main()
{
    vec3 N = normalize(pass_normal);
    vec3 L = normalize(light_position - pass_position_world);

    vec4 albedo = mix(texture(u_albedo_map, pass_uv), albedo_color, albedo_color.a);

    if (albedo.a < 0.001)
        discard;

    // write lighting information to texel
    vec3 voxel = (pass_position_world - u_world.xyz) / (0.5 * u_world.w); // normalize it to [-1, 1]
    voxel = 0.5 * voxel + vec3(0.5); // normalize to [0, 1]
    ivec3 dim = imageSize(u_albedo_texture);
    ivec3 coord = ivec3(dim * voxel);

    float shadow = in_shadow(pass_position_light);
    float diffuse = max(dot(N, L), 0.0);
    vec3 color = (1.0 - shadow) * (diffuse) * albedo.rgb;

    f16vec4 final_color = f16vec4(color.r, color.g, color.b, albedo.a);
    // f16vec4 final_color = f16vec4(color.r, color.g, color.b, 1.0);
    imageAtomicAdd(u_albedo_texture, coord, final_color);

    // TODO: average normal
    N = 0.5 * (N + 1.0); // normalize to [0, 1]
    f16vec4 normal_color = f16vec4(N.r, N.g, N.b, 1.0);
    imageAtomicAdd(u_normal_texture, coord, normal_color);
    // imageAtomicMax(u_normal_texture, coord, normal_color);
}
