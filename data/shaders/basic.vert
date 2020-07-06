#version 450 core
layout (location = 0) in vec3 in_position;

uniform mat4 u_PV;
uniform mat4 u_M;

void main()
{
    vec4 world_position = u_M * vec4(in_position, 1.0);
    gl_Position = u_PV * world_position;
}
