#version 450 core
layout (location = 0) in vec3 in_position;
uniform sampler3D u_voxel_texture;

out vec4 pass_color;

uniform mat4 PV;
uniform vec3 u_world_center;
uniform float u_unit_size;
uniform int u_voxel_dim;

void main()
{
    int x = gl_InstanceID % u_voxel_dim;
    int y = (gl_InstanceID / u_voxel_dim) % u_voxel_dim;
    int z = gl_InstanceID / (u_voxel_dim * u_voxel_dim);
    vec3 texCoord = vec3(float(x) / u_voxel_dim, float(y) / u_voxel_dim, float(z) / u_voxel_dim);
    vec4 color = texture(u_voxel_texture, texCoord);
    if (color.a == 0.0)
    {
        // move the voxel outside the clipping space
        gl_Position = vec4(vec3(-99999), 1.0);
    }
    else
    {
        vec3 world_pos = u_unit_size * ((in_position + vec3(0.5)) + vec3(float(x), float(y), float(z)));
        world_pos += u_world_center;
        // world_pos = in_position;
        gl_Position = PV * vec4(world_pos, 1.0);
        pass_color = color;
        // pass_color = vec4(float(x) / u_voxel_dim, float(y) / u_voxel_dim, float(z) / u_voxel_dim, 1.0);
    }
}
