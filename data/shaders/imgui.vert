#version 450 core
layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec4 in_color;

out vec2 pass_uv;
out vec4 pass_color;

uniform mat4 u_projection;

void main()
{
    pass_uv = in_uv;
    pass_color = in_color;
    gl_Position = u_projection * vec4(in_position, 0.0, 1.0);
}
