#version 450 core
in vec3 pass_position;
in vec3 pass_normal;
in vec2 pass_uv;

layout (location = 0) out vec4 out_color;

uniform sampler2D u_albedo_map;

layout (std140, binding = 1) uniform Light
{
    vec3 light_position;
    float padding;
};

layout (std140, binding = 2) uniform Material
{
    vec4 albedo_color;
};

void main()
{
    vec3 N = normalize(pass_normal);
    vec3 L = normalize(light_position - pass_position);

    vec4 albedo = mix(texture(u_albedo_map, pass_uv), albedo_color, albedo_color.a);

    if (albedo.a < 0.001)
        discard;

    float diffuse = max(dot(N, L), 0.0) + 0.2;

    out_color = vec4(diffuse * albedo.rgb, 1.0);
}
