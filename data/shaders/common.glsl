
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

float inShadow(const in vec4 position_light, float NdotL)
{
    vec3 coords = position_light.xyz / position_light.w;
    coords = 0.5 * coords + 0.5;

    float current_depth = coords.z;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadow_map, 0);

    float bias = max(0.05 * (1.0 - NdotL), 0.005);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            vec2 offset = vec2(x, y) * texelSize;
            float closest_depth = texture(u_shadow_map, coords.xy + offset).r;

            shadow += coords.z - bias > closest_depth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;
    return shadow;
}
