#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

out vec3 pass_positions;
out vec3 pass_normals;

void main()
{
    vec4 world_pos = vec4(in_position, 1.0); // assume no transformation
    // we only care about world space
    gl_Position = world_pos;
    // we still care about the interpolated fragment position and normal 
    pass_positions = world_pos.xyz;
    pass_normals = normalize(in_normal); // assume no transformation
}
