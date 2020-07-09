#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

layout (location = 0) out vec3 pass_position;
layout (location = 1) out vec3 pass_normal;
layout (location = 2) out vec2 pass_uv;

layout (std140, binding = 0) uniform Camera
{
    mat4 PV;
};

uniform mat4 M;

void main()
{
    vec4 world_position = M * vec4(in_position, 1.0);
    pass_position = world_position.xyz;
    pass_normal = mat3(M) * in_normal; // assume no scaling
    pass_uv = in_uv;
    gl_Position = PV * world_position;
}
