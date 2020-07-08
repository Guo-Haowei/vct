#version 450 core
#extension GL_NV_gpu_shader5: enable
#extension GL_NV_shader_atomic_float: enable
#extension GL_NV_shader_atomic_fp16_vector: enable

in vec3 pass_position; // fragment world position
in vec3 pass_normal; // fragment normal
in vec2 pass_uv; // fragment normal

layout (rgba16f, binding = 0) uniform image3D u_albedo_texture;
layout (rgba16f, binding = 1) uniform image3D u_normal_texture;

uniform vec4 u_world; // xyz : world center; w : world size
uniform vec3 u_light_pos;

// TODO: config light
const vec3 light_position = vec3(0.0, 100.0, 2.0);

struct Material
{
    vec4 albedo;
};

uniform Material u_material;
uniform sampler2D u_albedo_map;

void main()
{
    vec3 N = normalize(pass_normal);
    vec3 L = normalize(light_position - pass_position);
    float diffuse = max(dot(N, L), 0.0);
    vec3 color = (diffuse) * vec3(1.0);

    vec4 albedo;
    if (u_material.albedo.a > 0.001)
        albedo = u_material.albedo;
    else
        albedo = texture(u_albedo_map, pass_uv);

    if (albedo.a < 0.001)
        discard;

    // write lighting information to texel
    vec3 voxel = (pass_position - u_world.xyz) / (0.5 * u_world.w); // normalize it to [-1, 1]
    voxel = 0.5 * voxel + vec3(0.5); // normalize to [0, 1]
    ivec3 dim = imageSize(u_albedo_texture);
    ivec3 coord = ivec3(dim * voxel);

    f16vec4 final_color = f16vec4(albedo.r, albedo.g, albedo.b, albedo.a);
    // f16vec4 final_color = f16vec4(color.r, color.g, color.b, 1.0);
    imageAtomicAdd(u_albedo_texture, coord, final_color);

    // TODO: average normal
    N = 0.5 * (N + 1.0); // normalize to [0, 1]
    f16vec4 normal_color = f16vec4(N.r, N.g, N.b, 1.0);
    imageAtomicAdd(u_normal_texture, coord, normal_color);
    // imageAtomicMax(u_normal_texture, coord, normal_color);
}
