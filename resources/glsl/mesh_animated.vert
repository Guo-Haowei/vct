#include "cbuffer.glsl.h"

// @TODO: config input slot
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in ivec4 in_bone_id;
layout(location = 5) in vec4 in_bone_weight;

out struct PS_INPUT {
    vec3 position;
    vec2 uv;
    mat3 TBN;
} ps_in;

void main() {
    mat4 bone_matrix = c_bones[in_bone_id.x] * in_bone_weight.x;
    bone_matrix += c_bones[in_bone_id.y] * in_bone_weight.y;
    bone_matrix += c_bones[in_bone_id.z] * in_bone_weight.z;
    bone_matrix += c_bones[in_bone_id.w] * in_bone_weight.w;

    mat4 world_matrix = c_model_matrix * bone_matrix;
    vec4 world_position = world_matrix * vec4(in_position, 1.0);

    mat3 rotation = mat3(world_matrix);
    vec3 T = normalize(rotation * in_tangent);
    vec3 N = normalize(rotation * in_normal);
    vec3 B = cross(N, T);

    gl_Position = c_projection_view_matrix * world_position;

    ps_in.position = world_position.xyz;
    ps_in.uv = in_uv;
    ps_in.TBN = mat3(T, B, N);
}
