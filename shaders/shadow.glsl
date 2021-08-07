#define NUM_SAMPLES 10
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10

//------------------------------------------------------------------------------
// Random
//------------------------------------------------------------------------------
// https://blog.demofox.org/2020/05/25/casual-shadertoy-path-tracing-1-basic-camera-diffuse-emissive/
uint WangHash(inout uint state) {
  state = uint(state ^ uint(61)) ^ uint(state >> uint(16));
  state *= uint(9);
  state = state ^ (state >> 4);
  state *= uint(0x27d4eb2d);
  state = state ^ (state >> 15);
  return state;
}

// random number between 0.0f and 1.0f
float Random(inout uint state) {
#define UINT_MAX 4294967296.0
  return float(WangHash(state)) / UINT_MAX;
}

//------------------------------------------------------------------------------
// Shadow
//------------------------------------------------------------------------------

// no filter
float Shadow(sampler2D shadowMap, const in vec4 position_light, float NdotL) {
  vec3 coords = position_light.xyz / position_light.w;
  coords = 0.5 * coords + 0.5;

  float current_depth = coords.z;

  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

  float bias = max(0.05 * (1.0 - NdotL), 0.005);

#define SAMPLE_STEP 1

  for (int x = -SAMPLE_STEP; x <= SAMPLE_STEP; ++x) {
    for (int y = -SAMPLE_STEP; y <= SAMPLE_STEP; ++y) {
      vec2 offset = vec2(x, y) * texelSize;
      float closest_depth = texture(shadowMap, coords.xy + offset).r;

      shadow += coords.z - bias > closest_depth ? 1.0 : 0.0;
    }
  }

  const float samples = float(2 * SAMPLE_STEP + 1);
  shadow /= samples * samples;
  return 1.0 - shadow;
}

vec2 g_poissonDisk[NUM_SAMPLES];

// PCF
void PoissonDiskSamples(inout uint state) {
  const float ANGLE_STEP = TWO_PI * float(NUM_RINGS) / float(NUM_SAMPLES);
  const float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

  float angle = Random(state) * TWO_PI;
  float radius = INV_NUM_SAMPLES;
  float radiusStep = radius;

  for (int i = 0; i < NUM_SAMPLES; i++) {
    g_poissonDisk[i] = vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
    radius += radiusStep;
    angle += ANGLE_STEP;
  }
}

float PCF_Filter(sampler2D shadowMap, vec2 uv, float zReceiver,
                 float filterRadius) {
  float sum = 0.0;
  for (int i = 0; i < PCF_NUM_SAMPLES; i++) {
    float depth = texture2D(shadowMap, uv + g_poissonDisk[i] * filterRadius).r;
    if (zReceiver <= depth) {
      sum += 1.0;
    }
  }

  for (int i = 0; i < PCF_NUM_SAMPLES; i++) {
    float depth =
        texture2D(shadowMap, uv + -g_poissonDisk[i].yx * filterRadius).r;
    if (zReceiver <= depth) {
      sum += 1.0;
    }
  }

  return sum / (2.0 * float(PCF_NUM_SAMPLES));
}

float PCF(sampler2D shadowMap, vec4 coords, inout uint state) {
  coords /= coords.w;
  vec2 uv = 0.5 * coords.xy + 0.5;
  float zReceiver = coords.z;

  PoissonDiskSamples(state);
  return PCF_Filter(shadowMap, uv, zReceiver, 0.002);
}
