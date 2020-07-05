#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

out vec3 pass_positions;
out vec3 pass_normals;
out vec2 pass_uvs;

void main()
{
    // assume no transformation
    vec4 world_pos = vec4(in_position, 1.0);
    pass_positions = world_pos.xyz;
    pass_normals = normalize(in_normal);
    pass_uvs = in_uv;
    gl_Position = world_pos;
}
