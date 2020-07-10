#version 450 core
layout (location = 0) out vec4 out_color;

in vec4 pass_color;

uniform int u_is_albedo;

void main()
{
    if (u_is_albedo == 1)
    {
        float gamma = 2.2;
        vec3 color = pass_color.rgb;
        color = color / (color + 1.0);
        color = pow(color, vec3(gamma));
        out_color.rgb = color;
    }
    else
    {
        out_color.rgb = 0.5 * (pass_color.xyz + 1.0);
    }
}
