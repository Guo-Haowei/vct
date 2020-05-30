#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

out vec3 pass_position;
out vec3 pass_normal;
out vec2 pass_uv;

uniform mat4 PV;

void main()
{
    // assume no transformation
    vec4 worldPos = vec4(in_position, 1.0);
    pass_position = worldPos.xyz;
    pass_normal = normalize(in_normal);
    pass_uv = in_uv;
    gl_Position = PV * worldPos;
}
