#version 450 core
layout (location = 0) in vec2 in_position;

out vec2 pass_coord;

void main()
{
    pass_coord = 0.5 * in_position + vec2(0.5); // from [-1, 1] to [0, 1]
    // pass_coord.y = 1.0 - pass_coord.y;
    gl_Position = vec4(in_position, 0.0, 1.0);
}
