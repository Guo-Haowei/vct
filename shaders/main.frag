#version 450 core
in vec3 pass_position;
in vec3 pass_normal;
in vec2 pass_uv;

in VS_OUT
{
    vec3 pass_position;
    vec3 pass_normal;
    vec2 pass_uv;
    vec4 pass_lightspace_position;
} fs_in;

layout (location = 0) out vec4 out_color;

uniform vec3 u_light_pos;
uniform sampler2D u_albedo;
uniform sampler2D u_shadow;

#if 0
float calShadow(vec4 position)
{
    vec3 project_coords = position.xzy / position.w;
    project_coords = 0.5 * project_coords + vec3(0.5);
    float closest_depth = texture(u_shadow, project_coords.xy).r;
    float current_depth = project_coords.z;
    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    float bias = 0.05;  
    float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;

    return shadow;
}
#endif

void main()
{
    // float shadow = calShadow(fs_in.pass_lightspace_position);
    float shadow = 1.0;
    float diffuse = max(0, dot(normalize(fs_in.pass_normal), normalize(u_light_pos - fs_in.pass_position)));
    vec4 color = shadow * (diffuse) * texture(u_albedo, fs_in.pass_uv);
    out_color = vec4(color);
}
