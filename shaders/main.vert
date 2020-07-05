#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

out VS_OUT
{
    vec3 pass_position;
    vec3 pass_normal;
    vec2 pass_uv;
    vec4 pass_lightspace_position;
} vs_out;

uniform mat4 PV;
uniform mat4 u_light_pv;

void main()
{
    // assume no transformation
    vec4 world_pos = vec4(in_position, 1.0);
    vs_out.pass_position = world_pos.xyz;
    vs_out.pass_normal = normalize(in_normal);
    vs_out.pass_uv = in_uv;
    vs_out.pass_lightspace_position = u_light_pv * world_pos;
    gl_Position = PV * world_pos;
}
