#if !defined( __cplusplus )
#if !defined( _IS_GLSL_LANG_ )
#define _IS_HLSL_LANG_
#endif
#endif

#ifdef __cplusplus
#pragma once
#pragma warning( push )
#pragma warning( disable : 4324 )

#include "Core/GeomMath.hpp"
#include "Core/Image.hpp"
using sampler2D = uint64_t;
using sampler3D = uint64_t;

using TextureHandle = intptr_t;
using TextureFormat = intptr_t;

constexpr size_t CONSTANT_BUFFER_ALIGNMENT = 32;

struct TextureBase {
    TextureHandle handle{ 0 };
    TextureFormat format{ 0 };
    PIXEL_FORMAT pixelFormat{ PIXEL_FORMAT::UNKNOWN };

    uint32_t width{ 0 };
    uint32_t height{ 0 };
    uint32_t mips{ 1 };
    uint32_t samples{ 1 };
};

struct Texture2D : TextureBase {
};

struct MaterialTextures {
    Texture2D albedoMap;
    Texture2D normalMap;
    Texture2D pbrMap;
};

#endif

#if defined( __cplusplus )
#define CBUFFER( NAME, SLOT, REG ) struct alignas( CONSTANT_BUFFER_ALIGNMENT ) NAME
#elif defined( _IS_GLSL_LANG_ )
#define CBUFFER( NAME, SLOT, REG ) layout( std140, binding = SLOT ) uniform NAME
#elif defined( _IS_HLSL_LANG_ )
#define CBUFFER( NAME, SLOT, REG ) cbuffer NAME : register( REG )
#else
#error "Unknown language"
#endif

#if !defined( _IS_HLSL_LANG_ )
#define float2   vec3
#define float3   vec3
#define float4   vec4
#define float2x2 mat2
#define float3x3 mat3
#define float4x4 mat4
#endif

#if defined( _IS_GLSL_LANG_ )
uniform sampler2D UniformAlbedoMap;
uniform sampler2D UniformNormalMap;
uniform sampler2D UniformPBRMap;
#endif

CBUFFER( PerBatchConstants, 0, b0 )
{
    float4x4 Model;

    float4 AlbedoColor;

    float Metallic;
    float Roughness;
    float HasAlbedoMap;
    float HasPbrMap;

    float HasNormalMap;
    float TextureMapIdx;
    float ReflectPower;
    int _padint0;
};

CBUFFER( PerFrameConstants, 1, b1 )
{
    float4x4 View;
    float4x4 Proj;

    float3 CamPos;
    int DebugCSM;

    float3 SunDir;
    int EnableGI;

    float3 LightColor;
    float VoxelSize;

    float4 CascadedClipZ;
    float4x4 LightPV;

    float3 WorldCenter;
    float WorldSizeHalf;

    float TexelSize;
    int NoTexture;
    int ScreenWidth;
    int ScreenHeight;
};

#if !defined( _IS_HLSL_LANG_ )
// @TODO: get rid of this
#define NUM_OVERLAYS 4

CBUFFER( PerSceneConstants, 2, b2 )
{
    // @TODO remove this
    sampler2D ShadowMap;
    sampler3D VoxelAlbedoMap;
    sampler3D VoxelNormalMap;
    sampler3D VoxelEmissiveMap;

    float4 OverlayPositions[NUM_OVERLAYS];
};

#endif

#if defined( __cplusplus )
// CB size is required to be 256-byte aligned.
const size_t kSizePerBatchConstantBuffer = ALIGN( sizeof( PerBatchConstants ), CONSTANT_BUFFER_ALIGNMENT );
const size_t kSizePerFrameConstantBuffer = ALIGN( sizeof( PerFrameConstants ), CONSTANT_BUFFER_ALIGNMENT );
const size_t kSizePerSceneConstantBuffer = ALIGN( sizeof( PerSceneConstants ), CONSTANT_BUFFER_ALIGNMENT );
static_assert( sizeof( PerBatchConstants ) == kSizePerBatchConstantBuffer );
static_assert( sizeof( PerFrameConstants ) == kSizePerFrameConstantBuffer );
static_assert( sizeof( PerSceneConstants ) == kSizePerSceneConstantBuffer );
#pragma warning( pop )
#endif