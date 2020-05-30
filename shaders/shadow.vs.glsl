#version 450 core
layout (location = 0) in vec3 in_position;
uniform mat4 PV;

void main()
{
    gl_Position = PV * vec4(in_position, 1.0);
}
