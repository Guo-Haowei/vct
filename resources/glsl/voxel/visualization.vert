layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

layout(location = 0) out vec4 pass_color;

#include "cbuffer.glsl.h"

layout(rgba16f, binding = 0) uniform image3D u_albedo_texture;
layout(rgba16f, binding = 1) uniform image3D u_normal_texture;

void main() {
    int voxel_texture_size = imageSize(u_albedo_texture).x;
    int x = gl_InstanceID % voxel_texture_size;
    int y = (gl_InstanceID / voxel_texture_size) % voxel_texture_size;
    int z = gl_InstanceID / (voxel_texture_size * voxel_texture_size);
    vec4 color;
    if (c_debug_texture_id == 1) {
        color = imageLoad(u_albedo_texture, ivec3(x, y, z));
    } else {
        color = imageLoad(u_normal_texture, ivec3(x, y, z));
    }

    if (color.a < 0.0001) {
        // move the voxel outside the clipping space
        gl_Position = vec4(vec3(-99999), 1.0);
    } else {
        float world_size = 2.0 * c_world_size_half;
        float voxel_size = world_size / float(voxel_texture_size);
        vec3 coord_pos = vec3(float(x), float(y), float(z)) / float(voxel_texture_size);  // [0, 1]
        coord_pos = 2.0 * coord_pos - vec3(1.0);                                          // [-1, 1]
        vec3 offset = c_world_size_half * coord_pos + c_world_center + 0.5 * vec3(voxel_size);
        vec3 world_position = voxel_size * in_position + offset;
        gl_Position = c_projection_view_matrix * vec4(world_position, 1.0);

        pass_color = color;
    }
}
