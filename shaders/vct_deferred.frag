#define PI 3.14159265359

layout(location = 0) out vec4 out_color;
layout(location = 0) in vec2 pass_uv;

uniform sampler2D u_shadow_map;

uniform sampler2D u_gbuffer_albedo;
uniform sampler2D u_gbuffer_position_metallic;
uniform sampler2D u_gbuffer_normal_roughness;
uniform sampler2D u_gbuffer_depth;

uniform sampler3D u_voxel_albedo;
uniform sampler3D u_voxel_normal;

uniform int u_gi_mode;

#include "cbuffer.glsl"

layout(std140, binding = 3) uniform Constant {
  vec3 world_center;
  float world_size_half;
  float texel_size;
  float voxel_size;
};

struct DiffuseCone {
  vec3 direction;
  float weight;
};

const DiffuseCone g_diffuseCones[6] = DiffuseCone[6](
    DiffuseCone(vec3(0.0, 1.0, 0.0), PI / 4.0),
    DiffuseCone(vec3(0.0, 0.5, 0.866025), 3.0 * PI / 20.0),
    DiffuseCone(vec3(0.823639, 0.5, 0.267617), 3.0 * PI / 20.0),
    DiffuseCone(vec3(0.509037, 0.5, -0.7006629), 3.0 * PI / 20.0),
    DiffuseCone(vec3(-0.50937, 0.5, -0.7006629), 3.0 * PI / 20.0),
    DiffuseCone(vec3(-0.823639, 0.5, 0.267617), 3.0 * PI / 20.0));

#include "common.glsl"

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 traceCone(vec3 from, vec3 direction, float aperture) {
  float max_dist = 2.0 * world_size_half;
  vec4 acc = vec4(0.0);

  float offset = 2.0 * voxel_size;
  float dist = offset + voxel_size;

  while (acc.a < 1.0 && dist < max_dist) {
    vec3 conePosition = from + direction * dist;
    float diameter = 2.0 * aperture * dist;
    float mipLevel = log2(diameter / voxel_size);

    vec3 coords = (conePosition - world_center) / world_size_half;
    coords = 0.5 * coords + 0.5;

    vec4 voxel = textureLod(u_voxel_albedo, coords, mipLevel);
    acc += (1.0 - acc.a) * voxel;

    dist += 0.5 * diameter;
  }

  return acc.rgb;
}

vec3 indirectDiffuse(vec3 position, vec3 N) {
  const float aperture = 0.57735;

  vec3 diffuse = vec3(0.0);

  vec3 up = vec3(0.0, 1.0, 0.0);

  if (abs(dot(N, up)) > 0.999)
    up = vec3(0.0, 0.0, 1.0);

  vec3 T = normalize(up - dot(N, up) * N);
  vec3 B = cross(T, N);

  for (int i = 0; i < 6; ++i) {
    vec3 direction = T * g_diffuseCones[i].direction.x +
                     B * g_diffuseCones[i].direction.z + N;
    direction = normalize(direction);

    diffuse +=
        g_diffuseCones[i].weight * traceCone(position, direction, aperture);
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
  float depth = texture(u_gbuffer_depth, pass_uv).r;

  if (depth > 0.999)
    discard;

  gl_FragDepth = depth;

  vec4 normal_roughness = texture(u_gbuffer_normal_roughness, pass_uv);
  vec4 position_metallic = texture(u_gbuffer_position_metallic, pass_uv);
  vec3 world_position = position_metallic.xyz;
  float roughness = normal_roughness.w;
  float metallic = position_metallic.w;

  vec4 light_space_position = LightPV * vec4(world_position, 1.0);

  vec4 albedo = texture(u_gbuffer_albedo, pass_uv);
  vec3 F0 = mix(vec3(0.04), albedo.rgb, metallic);
  vec3 Lo = vec3(0.0);

  vec3 N = normal_roughness.xyz;
  vec3 L = normalize(LightPos - world_position);
  vec3 V = normalize(CamPos - world_position);
  vec3 H = normalize(V + L);

  vec3 radiance = LightColor;

  float NdotL = max(dot(N, L), 0.0);
  float NdotH = max(dot(N, H), 0.0);
  float NdotV = max(dot(N, V), 0.0);

  // direct
  {
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

    float shadow = inShadow(light_space_position, NdotL);

    vec3 directLight = (kD * albedo.rgb / PI + specular) * radiance * NdotL;
    Lo += (1.0 - shadow) * directLight;
  }

  float ambient = 0.15;

  vec3 color = Lo + ambient * albedo.rgb;

  // indirect light
  if (u_gi_mode == 1) {
    vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    // indirect diffuse
    vec3 diffuse = albedo.rgb * indirectDiffuse(world_position, N);

    // specular cone
    vec3 coneDirection = reflect(-V, N);
    vec3 specular =
        0.5 * indirectSpecular(world_position, coneDirection, roughness);
    specular = vec3(0.0);

    color += (kD * diffuse + specular);
  }

  float gamma = 2.2;
  color = color / (color + 1.0);
  color = pow(color, vec3(gamma));

  color = mix(color, vec3(0.9, 0.9, 0.8), 0.05);
  out_color = vec4(color, 1.0);
}
