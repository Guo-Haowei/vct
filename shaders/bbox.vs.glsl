#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_center;
layout (location = 2) in vec3 in_size;

uniform mat4 PV;

void main()
{
    vec3 worldPos = in_position * in_size + in_center;
    gl_Position = PV * vec4(worldPos, 1.0);
}

