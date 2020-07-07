#version 450 core
layout (location = 0) out vec4 out_color;

uniform vec3 u_color;

void main()
{
    out_color = vec4(u_color, 1.0);
}
