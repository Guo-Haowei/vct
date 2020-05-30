#version 450 core
layout (location = 0) out vec4 out_color;

in vec2 pass_position;

void main()
{
    out_color = vec4(0.5 * pass_position + vec2(0.5), 1.0, 1.0);
}

