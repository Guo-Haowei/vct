#version 450 core
layout (location = 0) in vec3 in_position;

uniform mat4 u_PVM;

void main()
{
    gl_Position = u_PVM * vec4(in_position, 1.0);
}
