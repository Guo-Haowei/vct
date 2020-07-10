#version 450 core
#define PI 3.14159265359

layout (location = 0) out vec4 out_color;
layout (location = 0) in vec2 pass_uv;

uniform sampler2D u_gbuffer_albedo;
uniform sampler2D u_gbuffer_position_metallic;
uniform sampler2D u_gbuffer_normal_roughness;
uniform sampler2D u_gbuffer_depth;

layout (std140, binding = 1) uniform FSPerFrame
{
    vec3 light_position; // direction
    float _per_frame_pad0;
    vec3 light_color;
    float _per_frame_pad1;
    vec3 camera_position;
    float _per_frame_pad2;
};

float distributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometrySmith(float NdotV, float NdotL, float roughness)
{
    return geometrySchlickGGX(NdotV, roughness) *
           geometrySchlickGGX(NdotL, roughness);
}

vec3 fresnelSchlick(float cosTheta, const in vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    float depth = texture(u_gbuffer_depth, pass_uv).r;

    if (depth > 0.999)
        discard;

    gl_FragDepth = depth;

    vec4 normal_roughness = texture(u_gbuffer_normal_roughness, pass_uv);
    vec4 position_metallic = texture(u_gbuffer_position_metallic, pass_uv);
    vec3 world_position = position_metallic.xyz;
    float roughness = normal_roughness.w;
    float metallic = position_metallic.w;

    vec4 albedo = texture(u_gbuffer_albedo, pass_uv);

    // if (albedo.a < 0.001)
    //     discard;

    vec3 F0 = mix(vec3(0.04), albedo.rgb, metallic);

    vec3 Lo = vec3(0.0);

    vec3 N = normal_roughness.xyz;
    vec3 V = normalize(camera_position - world_position);
    vec3 L = normalize(light_position - world_position);
    vec3 H = normalize(V + L);

    float dist_to_light = length(light_position - world_position);
    float attenuation = 1.0 / (dist_to_light);
    // vec3 radiance = light_color;
    vec3 radiance = attenuation * light_color;

    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    // cook-torrance brdf
    float NDF = distributionGGX(NdotH, roughness);
    float G = geometrySmith(NdotV, NdotL, roughness);
    vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 nom = NDF * G * F;
    float denom = 4 * NdotV * NdotL;

    vec3 specular = nom / max(denom, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL;

    float ao = 1.0;
    vec3 ambient = vec3(0.03) * albedo.rgb * ao;

    vec3 color = ambient + Lo;

    float gamma = 2.2;
    color = color / (color + 1.0);
    color = pow(color, vec3(gamma));

    out_color = vec4(color, 1.0);
}

