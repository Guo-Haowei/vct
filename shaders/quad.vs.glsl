#version 450 core
layout (location = 0) in vec2 in_position;

out vec2 pass_position;

void main()
{
    pass_position = in_position;
    gl_Position = vec4(in_position, 0.0, 1.0);
}
