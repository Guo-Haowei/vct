#ifndef NUM_CASCADES
#define NUM_CASCADES 3
#endif
#ifndef MAX_MATERIALS
#define MAX_MATERIALS 300
#endif
#ifndef MAX_LIGHT_ICON
#define MAX_LIGHT_ICON 4
#endif
#ifndef NUM_SSAO_KERNEL
#define NUM_SSAO_KERNEL 64
#endif

// constant buffer
#ifdef __cplusplus
#define CONSTANT_BUFFER(name, reg) \
    struct name : public ConstantBufferBase<reg>
template<int N>
struct ConstantBufferBase {
    constexpr int get_slot() { return N; }
};
#else
#define CONSTANT_BUFFER(name, reg) layout(std140, binding = reg) uniform name
#endif

// sampler
#ifdef __cplusplus
using sampler2D = uint64_t;
using sampler3D = uint64_t;
typedef struct {
    uint64_t data;
    uint64_t padding;
} Sampler2DArray;
#else
#define Sampler2DArray sampler2D
#endif

CONSTANT_BUFFER(PerFrameConstantBuffer, 0) {
    mat4 c_view_matrix;
    mat4 c_projection_matrix;
    mat4 c_projection_view_matrix;

    // @TODO: refactor names
    vec3 CamPos;
    int DebugCSM;

    vec3 SunDir;
    int EnableGI;

    vec3 LightColor;
    float VoxelSize;

    vec4 CascadedClipZ;
    mat4 LightPVs[NUM_CASCADES];

    vec3 WorldCenter;
    float WorldSizeHalf;

    int DebugTexture;
    int NoTexture;
    int ScreenWidth;
    int ScreenHeight;

    int SSAOKernelSize;
    float SSAOKernelRadius;
    int SSAONoiseSize;
    float TexelSize;

    vec2 padding0;
    int EnableSSAO;
    int EnableFXAA;
};

CONSTANT_BUFFER(PerBatchConstantBuffer, 1) {
    mat4 c_projection_view_model_matrix;
    mat4 c_model_matrix;
};

CONSTANT_BUFFER(MaterialConstantBuffer, 2) {
    vec4 AlbedoColor;
    float Metallic;
    float Roughness;
    int HasAlbedoMap;
    int HasPbrMap;

    int HasNormalMap;
    int TextureMapIdx;
    float ReflectPower;
    int _padint0;
};

CONSTANT_BUFFER(PerSceneConstantBuffer, 3) {
    vec4 SSAOKernels[NUM_SSAO_KERNEL];
    sampler2D ShadowMap;
    sampler3D VoxelAlbedoMap;
    sampler3D VoxelNormalMap;
    sampler3D VoxelEmissiveMap;
    sampler2D GbufferAlbedoMap;
    sampler2D GbufferPositionMetallicMap;
    sampler2D GbufferNormalRoughnessMap;
    sampler2D GbufferDepthMap;
    sampler2D SSAOMap;
    sampler2D NoiseMap;
    sampler2D FinalImage;
    sampler2D FXAA;
    Sampler2DArray LightIconTextures[MAX_LIGHT_ICON];
    Sampler2DArray AlbedoMaps[MAX_MATERIALS];
    Sampler2DArray NormalMaps[MAX_MATERIALS];
    Sampler2DArray PbrMaps[MAX_MATERIALS];
};

#ifdef __cplusplus
static_assert(sizeof(PerFrameConstantBuffer) % 16 == 0);
static_assert(sizeof(PerBatchConstantBuffer) % 16 == 0);
static_assert(sizeof(MaterialConstantBuffer) % 16 == 0);
static_assert(sizeof(PerSceneConstantBuffer) % 16 == 0);
#endif