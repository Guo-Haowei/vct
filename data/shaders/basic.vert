#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

out vec3 pass_position;
out vec3 pass_normal;
out vec2 pass_uv;

uniform mat4 u_PV;
uniform mat4 u_M;

void main()
{
    vec4 world_position = u_M * vec4(in_position, 1.0);
    pass_position = world_position.xyz;
    pass_normal = mat3(u_M) * in_normal;
    pass_uv = in_uv;
    gl_Position = u_PV * world_position;
}
