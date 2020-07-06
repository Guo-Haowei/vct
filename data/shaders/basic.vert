#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

layout (location = 0) out vec3 pass_position;
layout (location = 1) out vec3 pass_normal;

uniform mat4 u_PV;
uniform mat4 u_M;

void main()
{
    vec4 world_position = u_M * vec4(in_position, 1.0);
    pass_position = world_position.xyz;
    pass_normal = mat3(u_M) * in_normal;
    gl_Position = u_PV * world_position;
}
