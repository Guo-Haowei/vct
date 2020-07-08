#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

out vec3 pass_positions_world;
out vec4 pass_positions_light;
out vec3 pass_normals;
out vec2 pass_uvs;

uniform mat4 u_M;

layout (std140, binding = 1) uniform Light
{
    vec3 light_position;
    float padding;
    mat4 light_space;
};

void main()
{
    // assume no transformation
    vec4 world_position = u_M * vec4(in_position, 1.0);
    pass_positions_world = world_position.xyz;
    pass_positions_light = light_space * world_position;
    pass_normals = normalize(in_normal);
    pass_uvs = in_uv;
    gl_Position = world_position;
}
