#version 450 core
in vec3 pass_position_world;
in vec4 pass_position_light;
in vec3 pass_normal;
in vec2 pass_uv;

layout (location = 0) out vec4 out_color;

uniform sampler2D u_albedo_map;
uniform sampler2D u_shadow_map;

layout (std140, binding = 1) uniform Light
{
    vec3 light_position;
    float padding;
    mat4 light_space;
};

layout (std140, binding = 2) uniform Material
{
    vec4 albedo_color;
};

float in_shadow(vec4 position_light)
{
    vec3 coords = position_light.xyz / position_light.w;
    coords = 0.5 * coords + 0.5;
    float closest_depth = texture(u_shadow_map, coords.xy).r;
    float current_depth = coords.z;

    float bias = 0.005;
    return current_depth - bias > closest_depth ? 1.0 : 0.0;
}

void main()
{
    vec3 N = normalize(pass_normal);
    vec3 L = normalize(light_position - pass_position_world);

    vec4 albedo = mix(texture(u_albedo_map, pass_uv), albedo_color, albedo_color.a);

    if (albedo.a < 0.001)
        discard;

    float shadow = in_shadow(pass_position_light);
    float diffuse = max(dot(N, L), 0.0);
    vec3 color = (1.0 - shadow) * (diffuse) * albedo.rgb;

    out_color = vec4(color, 1.0);
}
