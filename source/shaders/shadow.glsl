//------------------------------------------------------------------------------
// Shadow
//------------------------------------------------------------------------------
#ifndef NUM_CASCADES
#define NUM_CASCADES 3
#endif

// no filter
float Shadow(sampler2D shadowMap, const in vec4 position_light, float NdotL
             //, int level
) {
    vec3 coords = position_light.xyz / position_light.w;
    coords = 0.5 * coords + 0.5;

    // coords.x /= float( NUM_CASCADES );
    // coords.x += float( level ) / float( NUM_CASCADES );

    float current_depth = coords.z;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    float bias = max(0.005 * (1.0 - NdotL), 0.0005);

    const int SAMPLE_STEP = 1;
    for (int x = -SAMPLE_STEP; x <= SAMPLE_STEP; ++x) {
        for (int y = -SAMPLE_STEP; y <= SAMPLE_STEP; ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            float closest_depth = texture(shadowMap, coords.xy + offset).r;

            shadow += coords.z - bias > closest_depth ? 1.0 : 0.0;
        }
    }

    const float samples = float(2 * SAMPLE_STEP + 1);
    shadow /= samples * samples;
    return shadow;
}
