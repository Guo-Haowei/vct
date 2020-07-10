#version 450 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_bitangent;

layout (location = 0) out vec3 pass_position;
layout (location = 1) out vec2 pass_uv;
layout (location = 2) out mat3 pass_TBN;

layout (std140, binding = 0) uniform VSPerFrame
{
    mat4 PV;
    mat4 lightSpace;
};

uniform mat4 M;

void main()
{
    vec4 world_position = M * vec4(in_position, 1.0);
    pass_position = world_position.xyz;
    mat3 rotation = mat3(M);
    vec3 T = normalize(rotation * in_tangent);
    vec3 B = normalize(rotation * in_bitangent);
    vec3 N = normalize(rotation * in_normal);

    gl_Position = PV * world_position;
    pass_uv = in_uv;
    pass_TBN = mat3(T, B, N);
}
