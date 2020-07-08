#version 450 core
layout (location = 0) out vec4 out_color;

in vec2 pass_uv;

uniform sampler2D u_depth_map;

void main()
{
    out_color = vec4(texture(u_depth_map, pass_uv).rrr, 1.0);
}
