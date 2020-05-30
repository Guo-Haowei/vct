#version 450 core
in vec3 pass_position; // fragment world position
in vec3 pass_normal; // fragment normal
in vec2 pass_uv; // fragment normal
// layout (location = 0) out vec4 out_color;
layout (RGBA8) uniform image3D u_voxel_texture;

uniform vec3 u_world_center;
uniform float u_world_size_half;
uniform sampler2D u_albedo;

void main()
{
    // vec3 lightPos = vec3(0, 2000, 3);
    // float diffuse = max(0, dot(normalize(pass_normal), normalize(lightPos - pass_position)));
    vec3 lightDir = normalize(vec3(0, 1, 1));
    float diffuse = max(0, dot(normalize(pass_normal), lightDir));
    vec4 color = (diffuse) * texture(u_albedo, pass_uv);

    // write lighting information to texel
    vec3 voxel = (pass_position - u_world_center) / u_world_size_half; // normalize it to [-1, 1]
    voxel = 0.5 * voxel + vec3(0.5); // normalize to [0, 1]
    ivec3 dim = imageSize(u_voxel_texture);
    // float alpha = pow(1 - material.transparency, 4); // For soft shadows to work better with transparent materials.
    // ignore transparency for now
    ivec3 coord = ivec3(dim * voxel);
    // need to accumulate alpha
    vec4 acummulated_color = imageLoad(u_voxel_texture, coord).rgba;
    vec4 final_color = vec4(color + acummulated_color);
    // a texel is affected by multiple texels, calculate average of it?
    imageStore(u_voxel_texture, coord, final_color);
}
