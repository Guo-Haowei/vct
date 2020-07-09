#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

out vec3 pass_position_world;
out vec4 pass_position_light;
out vec3 pass_normal;
out vec2 pass_uv;

layout (std140, binding = 0) uniform Camera
{
    mat4 PV;
};

layout (std140, binding = 1) uniform Light
{
    vec3 light_position;
    float padding;
    mat4 light_space;
};

uniform mat4 u_M;

void main()
{
    vec4 world_position = u_M * vec4(in_position, 1.0);
    pass_position_world = world_position.xyz;
    pass_position_light = light_space * world_position;
    pass_normal = mat3(u_M) * in_normal;
    pass_uv = in_uv;
    gl_Position = PV * world_position;
}
