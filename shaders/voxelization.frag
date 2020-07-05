#version 450 core
#extension GL_NV_gpu_shader5: enable
#extension GL_NV_shader_atomic_float: enable
#extension GL_NV_shader_atomic_fp16_vector: enable

in vec3 pass_position; // fragment world position
in vec3 pass_normal; // fragment normal
in vec2 pass_uv; // fragment normal
// layout (location = 0) out vec4 out_color;

layout (rgba16f) uniform image3D u_voxel_texture;

uniform vec3 u_world_center;
uniform float u_world_size_half;
uniform sampler2D u_albedo;
uniform vec3 u_light_pos;

void main()
{
    float diffuse = max(0, dot(normalize(pass_normal), normalize(u_light_pos - pass_position)));
    vec4 color = (diffuse) * texture(u_albedo, pass_uv);

    // write lighting information to texel
    vec3 voxel = (pass_position - u_world_center) / u_world_size_half; // normalize it to [-1, 1]
    voxel = 0.5 * voxel + vec3(0.5); // normalize to [0, 1]
    ivec3 dim = imageSize(u_voxel_texture);
    // float alpha = pow(1 - material.transparency, 4); // For soft shadows to work better with transparent materials.
    // ignore transparency for now
    ivec3 coord = ivec3(dim * voxel);
    // need to accumulate alpha
    // vec4 max_so_far = imageLoad(u_voxel_texture, coord).rgba;
    // float r = max(max_so_far.r, color.r);
    // float g = max(max_so_far.g, color.g);
    // float b = max(max_so_far.b, color.b);
    f16vec4 final_color = f16vec4(color.r, color.g, color.b, 1.0);
    // vec4 final_color = vec4(color);
    // a texel is affected by multiple texels, calculate average of it?
    // imageStore(u_voxel_texture, coord, final_color);
    // imageAtomicMax(u_voxel_texture, coord, final_color);
    imageAtomicAdd(u_voxel_texture, coord, final_color);
}
