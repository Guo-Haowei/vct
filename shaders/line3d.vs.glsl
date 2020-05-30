#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_color;
uniform mat4 PV;
out vec3 pass_color;

void main()
{
    pass_color = in_color;
    gl_Position = PV * vec4(5000 * in_position, 1.0);
}
