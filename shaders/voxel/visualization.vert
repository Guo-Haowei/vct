#extension GL_NV_gpu_shader5 : enable
#extension GL_NV_shader_atomic_float : enable
#extension GL_NV_shader_atomic_fp16_vector : enable
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

layout(location = 0) out vec4 pass_color;

layout(rgba16f, binding = 0) uniform image3D u_voxel_texture;

#include "cbuffer.glsl"

layout(std140, binding = 3) uniform Constant {
  vec3 world_center;
  float world_size_half;
  float texel_size;
  // float voxel_size;
};

void main() {
  int voxel_texture_size = imageSize(u_voxel_texture).x;
  int x = gl_InstanceID % voxel_texture_size;
  int y = (gl_InstanceID / voxel_texture_size) % voxel_texture_size;
  int z = gl_InstanceID / (voxel_texture_size * voxel_texture_size);
  vec4 color = imageLoad(u_voxel_texture, ivec3(x, y, z));
  if (color.a < 0.0001) {
    // move the voxel outside the clipping space
    gl_Position = vec4(vec3(-99999), 1.0);
  } else {
    float world_size = 2.0 * world_size_half;
    float voxel_size = world_size / float(voxel_texture_size);
    vec3 coord_pos = vec3(float(x), float(y), float(z)) /
                     float(voxel_texture_size); // [0, 1]
    coord_pos = 2.0 * coord_pos - vec3(1.0);    // [-1, 1]
    vec3 offset =
        world_size_half * coord_pos + world_center + 0.5 * vec3(voxel_size);
    vec3 world_position = voxel_size * in_position + offset;
    gl_Position = PV * vec4(world_position, 1.0);

    pass_color = color;
  }
}
