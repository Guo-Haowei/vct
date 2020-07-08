#version 450 core
in vec3 pass_position;
in vec3 pass_normal;
in vec2 pass_uv;

layout (location = 0) out vec4 out_color;

struct Material
{
    vec4 albedo;
};

uniform Material u_material;
uniform sampler2D u_albedo_map;

// TODO: config light
const vec3 light_position = vec3(0.0, 100.0, 2.0);

void main()
{
    vec3 N = normalize(pass_normal);
    vec3 L = normalize(light_position - pass_position);

    vec4 albedo;
    if (u_material.albedo.a > 0.001)
        albedo = u_material.albedo;
    else
        albedo = texture(u_albedo_map, pass_uv);

    if (albedo.a < 0.001)
        discard;

    float diffuse = max(dot(N, L), 0.0) + 0.2;

    out_color = vec4(diffuse * albedo.rgb, 1.0);
}
