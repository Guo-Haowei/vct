#ifdef __cplusplus
#define CBUFFER( NAME, SLOT ) struct NAME
#else
#define CBUFFER( NAME, SLOT ) layout( std140, binding = SLOT ) uniform NAME
#endif

#ifdef __cplusplus
#pragma once
#include "Core/GeomMath.hpp"
#include "Core/Image.hpp"
using sampler2D = uint64_t;
using sampler3D = uint64_t;

using TextureHandle = intptr_t;
using TextureFormat = intptr_t;

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

CBUFFER( PerBatchConstants, 0 )
{
    mat4 Model;

    vec4 AlbedoColor;

    float Metallic;
    float Roughness;
    float HasAlbedoMap;
    float HasPbrMap;

    float HasNormalMap;
    float TextureMapIdx;
    float ReflectPower;
    int _padint0;
};

CBUFFER( PerFrameConstants, 1 )
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

    float TexelSize;
    int NoTexture;
    int ScreenWidth;
    int ScreenHeight;
};

#ifndef __cplusplus
uniform sampler2D UniformAlbedoMap;
uniform sampler2D UniformNormalMap;
uniform sampler2D UniformPBRMap;
#endif

// @TODO: get rid of this
#define NUM_OVERLAYS 4

#ifdef __cplusplus
struct ConstantCB
#else
#define Sampler2DArray sampler2D
layout( std140, binding = 3 ) uniform ConstantCB
#endif
{
    sampler2D ShadowMap;
    sampler3D VoxelAlbedoMap;
    sampler3D VoxelNormalMap;
    sampler3D VoxelEmissiveMap;

    vec4 OverlayPositions[NUM_OVERLAYS];
};

#ifdef __cplusplus
static_assert( sizeof( ConstantCB ) % 16 == 0 );

// CB size is required to be 256-byte aligned.
const size_t kSizePerBatchConstantBuffer = ALIGN( sizeof( PerBatchConstants ), 256 );
const size_t kSizePerFrameConstantBuffer = ALIGN( sizeof( PerFrameConstants ), 256 );
#endif