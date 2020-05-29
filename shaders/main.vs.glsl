#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

out vec3 pass_position;
out vec3 pass_normal;

uniform mat4 PV;

void main()
{
    vec4 worldPos = vec4(in_position, 1.0); // assume no transformation
    pass_position = worldPos.xyz;
    pass_normal = normalize(in_normal); // assume no transformation

    gl_Position = PV * worldPos;
}
