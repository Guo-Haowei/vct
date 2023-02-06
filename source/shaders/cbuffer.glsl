#ifdef __cplusplus
#include "Base/Defines.h"
#pragma once
#endif
#ifndef MAX_MATERIALS
#define MAX_MATERIALS 300
#endif
#ifndef NUM_SSAO_KERNEL
#define NUM_SSAO_KERNEL 64
#endif

#ifdef __cplusplus
#define CBUFFER( NAME, SLOT ) struct NAME
#else
#define CBUFFER( NAME, SLOT ) layout( std140, binding = SLOT ) uniform NAME
#endif

// struct TextureBase {
//     TextureHandler handler = 0;
//     TextureFormat format = 0;
//     PIXEL_FORMAT pixel_format;

//     uint32_t width = 0;
//     uint32_t height = 0;
//     uint32_t mips = 1;
//     uint32_t samples = 1;
// };

// struct TextureArrayBase : virtual TextureBase {
//     uint32_t size = 0;
// };

// struct Texture2D : virtual TextureBase {
// };

// struct TextureCube : virtual TextureBase {
// };

// struct Texture2DArray : Texture2D, TextureArrayBase {
// };

// struct TextureCubeArray : TextureCube, TextureArrayBase {
// };

// struct material_textures {
//     Texture2D diffuseMap;
//     Texture2D normalMap;
//     Texture2D metallicMap;
//     Texture2D roughnessMap;
//     Texture2D aoMap;
//     Texture2D heightMap;
// };

CBUFFER( PerFrameConstants, 0 )
{
    mat4 View;
    mat4 Proj;

    vec3 CamPos;
    int DebugCSM;

    vec3 SunDir;
    int EnableGI;

    vec3 LightColor;
    float VoxelSize;

    vec4 CascadedClipZ;
    mat4 LightPV;

    vec3 WorldCenter;
    float WorldSizeHalf;

    int _placeholder0;
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

CBUFFER( PerBatchConstants, 1 )
{
    mat4 Model;
};

#ifdef __cplusplus
struct MaterialCB
#else
layout( std140, binding = 2 ) uniform MaterialCB
#endif
{
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

#define NUM_OVERLAYS 8

#ifdef __cplusplus
using sampler2D = uint64_t;
using sampler3D = uint64_t;
typedef struct {
    uint64_t data;
    uint64_t padding;
} Sampler2DArray;
struct ConstantCB
#else
#define Sampler2DArray sampler2D
layout( std140, binding = 3 ) uniform ConstantCB
#endif
{
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
    Sampler2DArray AlbedoMaps[MAX_MATERIALS];
    Sampler2DArray NormalMaps[MAX_MATERIALS];
    Sampler2DArray PbrMaps[MAX_MATERIALS];

    vec4 OverlayPositions[NUM_OVERLAYS];
};

#ifdef __cplusplus
static_assert( sizeof( MaterialCB ) % 16 == 0 );
static_assert( sizeof( ConstantCB ) % 16 == 0 );

// CB size is required to be 256-byte aligned.
const size_t kSizePerBatchConstantBuffer = ALIGN( sizeof( PerBatchConstants ), 256 );
const size_t kSizePerFrameConstantBuffer = ALIGN( sizeof( PerFrameConstants ), 256 );
#endif