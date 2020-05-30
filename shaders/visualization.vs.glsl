#version 450 core
layout (location = 0) in vec3 in_position;
// can be defined at compile time
uniform vec3 u_world_center;
uniform float u_unit_size;
// per frame
uniform sampler3D u_voxel_texture;
uniform mat4 PV;

out vec4 pass_color;

void main()
{
    int voxel_dim = textureSize(u_voxel_texture, 0).x;
    int x = gl_InstanceID % voxel_dim;
    int y = (gl_InstanceID / voxel_dim) % voxel_dim;
    int z = gl_InstanceID / (voxel_dim * voxel_dim);
    vec3 texCoord = vec3(float(x), float(y), float(z)) / float(voxel_dim);
    vec4 color = texture(u_voxel_texture, texCoord);
    if (color.a == 0.0)
    {
        // move the voxel outside the clipping space
        gl_Position = vec4(vec3(-99999), 1.0);
    }
    else
    {
        float world_size_half = u_unit_size * voxel_dim * 0.5;
        vec3 coord_pos = vec3(float(x), float(y), float(z)) / float(voxel_dim); // [0, 1]
        coord_pos = 2.0 * coord_pos - vec3(1.0); // [-1, 1]
        vec3 offset = world_size_half * coord_pos + u_world_center;
        vec3 world_pos = u_unit_size * in_position + offset;
        gl_Position = PV * vec4(world_pos, 1.0);
        pass_color = color;
    }
}
