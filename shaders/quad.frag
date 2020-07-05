#version 450 core
layout (location = 0) out vec4 out_color;

in vec2 pass_coord;

uniform sampler2D u_texture;

void main()
{
    // out_color = vec4(0.5 * pass_position + vec2(0.5), 1.0, 1.0);
    float r = texture(u_texture, pass_coord).r;
    out_color = vec4(vec3(r), 1.0);
}

