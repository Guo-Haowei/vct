#include "cbuffer.glsl.h"

layout(location = 0) in vec3 in_position;
layout(location = 4) in ivec4 in_bone_id;
layout(location = 5) in vec4 in_bone_weight;

void main() {
    mat4 bone_matrix = c_bones[in_bone_id.x] * in_bone_weight.x;
    bone_matrix += c_bones[in_bone_id.y] * in_bone_weight.y;
    bone_matrix += c_bones[in_bone_id.z] * in_bone_weight.z;
    bone_matrix += c_bones[in_bone_id.w] * in_bone_weight.w;

    gl_Position = c_projection_view_model_matrix * bone_matrix * vec4(in_position, 1.0);
}
