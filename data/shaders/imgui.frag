#version 450 core
in vec2 pass_uv;
in vec4 pass_color;

uniform sampler2D u_texture;

layout (location = 0) out vec4 out_color;

void main()
{
    out_color = pass_color * texture(u_texture, pass_uv.xy);
}
