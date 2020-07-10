#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

out vec3 pass_positions;
out vec3 pass_normals;
out vec2 pass_uvs;
out vec4 pass_light_space_positions;

layout (std140, binding = 0) uniform VSPerFrame
{
    mat4 PV;
    mat4 lightSpace;
};

uniform mat4 u_M;

void main()
{
    // assume no transformation
    vec4 world_position = u_M * vec4(in_position, 1.0);
    pass_positions = world_position.xyz;
    pass_normals = normalize(in_normal);
    pass_uvs = in_uv;
    pass_light_space_positions = lightSpace * world_position;
    gl_Position = world_position;
}
