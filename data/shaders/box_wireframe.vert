#version 450 core
layout (location = 0) in vec3 in_position;

uniform mat4 u_PV;
uniform vec3 u_center;
uniform vec3 u_size;

void main()
{
    vec3 position = in_position * u_size + u_center;
    vec4 world_position = vec4(position, 1.0);
    gl_Position = u_PV * world_position;
}

