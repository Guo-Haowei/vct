#version 450 core
in vec3 pass_position; // fragment world position
in vec3 pass_normal; // fragment normal
// layout (location = 0) out vec4 out_color;
layout (RGBA8) uniform image3D u_voxel_texture;

uniform vec3 u_world_center;
uniform float u_world_size_half;

void main()
{
    vec3 lightPos = vec3(0, 2000, 3);
    float diffuse = max(0, dot(normalize(pass_normal), normalize(lightPos - pass_position)));
    float ambient = 0.2;
    vec3 color = (ambient + diffuse) * vec3(0.8);

    // write lighting information to texel
    vec3 voxel = (pass_position - u_world_center) / u_world_size_half; // normalize it to [-1, 1]
    voxel = 0.5 * voxel + vec3(0.5); // normalize to [0, 1]
    ivec3 dim = imageSize(u_voxel_texture);
    // float alpha = pow(1 - material.transparency, 4); // For soft shadows to work better with transparent materials.
    // ignore transparency for now
    vec4 final_color = vec4(color, 1.0);
    imageStore(u_voxel_texture, ivec3(dim * voxel), final_color);
}
