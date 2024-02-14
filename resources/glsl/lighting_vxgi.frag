#include "cbuffer.glsl.h"

layout(location = 0) out vec4 out_color;
layout(location = 0) in vec2 pass_uv;

#include "common.glsl"
#include "pbr.glsl"
#include "shadow.glsl"

struct DiffuseCone {
    vec3 direction;
    float weight;
};

const DiffuseCone g_diffuseCones[6] =
    DiffuseCone[6](DiffuseCone(vec3(0.0, 1.0, 0.0), PI / 4.0), DiffuseCone(vec3(0.0, 0.5, 0.866025), 3.0 * PI / 20.0),
                   DiffuseCone(vec3(0.823639, 0.5, 0.267617), 3.0 * PI / 20.0),
                   DiffuseCone(vec3(0.509037, 0.5, -0.7006629), 3.0 * PI / 20.0),
                   DiffuseCone(vec3(-0.50937, 0.5, -0.7006629), 3.0 * PI / 20.0),
                   DiffuseCone(vec3(-0.823639, 0.5, 0.267617), 3.0 * PI / 20.0));

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 traceCone(vec3 from, vec3 direction, float aperture) {
    float max_dist = 2.0 * c_world_size_half;
    vec4 acc = vec4(0.0);

    float offset = 2.0 * c_voxel_size;
    float dist = offset + c_voxel_size;

    while (acc.a < 1.0 && dist < max_dist) {
        vec3 conePosition = from + direction * dist;
        float diameter = 2.0 * aperture * dist;
        float mipLevel = log2(diameter / c_voxel_size);

        vec3 coords = (conePosition - c_world_center) / c_world_size_half;
        coords = 0.5 * coords + 0.5;

        vec4 voxel = textureLod(c_voxel_map, coords, mipLevel);
        acc += (1.0 - acc.a) * voxel;

        dist += 0.5 * diameter;
    }

    return acc.rgb;
}

vec3 indirectDiffuse(vec3 position, vec3 N) {
    const float aperture = 0.57735;

    vec3 diffuse = vec3(0.0);

    vec3 up = vec3(0.0, 1.0, 0.0);

    if (abs(dot(N, up)) > 0.999) up = vec3(0.0, 0.0, 1.0);

    vec3 T = normalize(up - dot(N, up) * N);
    vec3 B = cross(T, N);

    for (int i = 0; i < 6; ++i) {
        vec3 direction = T * g_diffuseCones[i].direction.x + B * g_diffuseCones[i].direction.z + N;
        direction = normalize(direction);

        diffuse += g_diffuseCones[i].weight * traceCone(position, direction, aperture);
    }

    return diffuse;
}

vec3 indirectSpecular(vec3 position, vec3 direction, float roughness) {
    // TODO: brdf lookup
    float aperture = 0.0374533;

    aperture = clamp(tan(0.5 * PI * roughness), aperture, 0.5 * PI);

    vec3 specular = traceCone(position, direction, aperture);

    return specular;
}

void main() {
    const vec2 uv = pass_uv;
    float depth = texture(c_gbuffer_depth_map, uv).r;

    if (depth > 0.999) discard;

    gl_FragDepth = depth;

    const vec4 normal_roughness = texture(c_gbuffer_normal_roughness_map, uv);
    const vec4 position_metallic = texture(c_gbuffer_position_metallic_map, uv);
    const vec4 worldPos = vec4(position_metallic.xyz, 1.0);
    float roughness = normal_roughness.w;
    float metallic = position_metallic.w;

    vec4 albedo = texture(c_gbuffer_albedo_map, uv);
    vec3 F0 = mix(vec3(0.04), albedo.rgb, metallic);
    vec3 Lo = vec3(0.0);

    if (c_no_texture != 0) {
        albedo.rgb = vec3(0.6);
    }

    const vec3 N = normal_roughness.xyz;
    const vec3 L = c_sun_direction;
    const vec3 V = normalize(c_camera_position - worldPos.xyz);
    const vec3 H = normalize(V + L);

    const vec3 radiance = c_light_color;

    const float NdotL = max(dot(N, L), 0.0);
    const float NdotH = max(dot(N, H), 0.0);
    const float NdotV = max(dot(N, V), 0.0);

    // direct
    // cook-torrance brdf
    const float NDF = distributionGGX(NdotH, roughness);
    const float G = geometrySmith(NdotV, NdotL, roughness);
    const vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    const vec3 nom = NDF * G * F;
    const float denom = 4 * NdotV * NdotL;

    vec3 specular = nom / max(denom, 0.001);

    const vec3 kS = F;
    const vec3 kD = (1.0 - metallic) * (vec3(1.0) - kS);

    vec3 directLight = (kD * albedo.rgb / PI + specular) * radiance * NdotL;

    float shadow = 0.0;
#if ENABLE_CSM
    // float clipSpaceZ = ( c_projection_view_matrix * worldPos ).z;
    // for ( int idx = 0; idx < NUM_CASCADES; ++idx )
    // {
    //     if ( clipSpaceZ <= c_cascade_clip_z[idx + 1] )
    //     {
    //         vec4 lightSpacePos = c_light_matricies[idx] * worldPos;
    //         shadow             = Shadow( c_shadow_map, lightSpacePos, NdotL, idx );
    //         break;
    //     }
    // }
#else
    vec4 lightSpacePos = c_light_matricies[0] * worldPos;
    shadow = Shadow(c_shadow_map, lightSpacePos, NdotL);
#endif
    Lo += (1.0 - shadow) * directLight;

    const float ao = c_enable_ssao == 0 ? 1.0 : texture(c_ssao_map, uv).r;

    if (c_enable_vxgi == 1)
    // indirect light
    {
        const vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);
        const vec3 kS = F;
        const vec3 kD = (1.0 - kS) * (1.0 - metallic);

        // indirect diffuse
        vec3 diffuse = albedo.rgb * indirectDiffuse(worldPos.xyz, N);

        // specular cone
        vec3 coneDirection = reflect(-V, N);
        vec3 specular = metallic * indirectSpecular(worldPos.xyz, coneDirection, roughness);
        Lo += (kD * diffuse + specular) * ao;
        // HACK
        Lo += 0.2 * albedo.rgb;  // ambient
    }

    vec3 color = Lo;

    const float gamma = 2.2;

    color = color / (color + 1.0);
    color = pow(color, vec3(gamma));

    out_color = vec4(color, 1.0);

#if ENABLE_CSM
    if (c_debug_csm != 0) {
        vec3 mask = vec3(0.1);
        for (int idx = 0; idx < NUM_CASCADES; ++idx) {
            if (clipSpaceZ <= c_cascade_clip_z[idx + 1]) {
                mask[idx] = 0.7;
                break;
            }
        }
        out_color.rgb = mix(out_color.rgb, mask, 0.1);
    }
#endif
}
