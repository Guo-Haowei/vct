#version 450 core
layout (location = 0) out vec4 out_color;

in vec4 pass_color;

void main()
{
    // out_color = vec4(1.0);
    if (pass_color.a == 0.0)
        discard;
    out_color = pass_color;
}
