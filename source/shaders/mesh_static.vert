#include "cbuffer.glsl.h"
#include "vsinput.glsl.h"

out struct PS_INPUT {
    vec3 position;
    vec2 uv;
    mat3 TBN;
} ps_in;

void main() {
    mat4 world_matrix = c_model_matrix;
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
