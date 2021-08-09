#pragma once
#include "universal/universal.h"

inline constexpr int NUM_CASCADES            = 3;
inline constexpr int IMAGE_VOXEL_ALBEDO_SLOT = 0;
inline constexpr int IMAGE_VOXEL_NORMAL_SLOT = 1;
inline constexpr int IMAGE_SSAO_SLOT         = 2;

// constexpr unsigned int VOXEL_TEXTURE_SIZE      = 64;
// constexpr unsigned int VOXEL_TEXTURE_MIP_LEVEL = log2( VOXEL_TEXTURE_SIZE );

// static_assert( VOXEL_TEXTURE_SIZE <= 256 );

enum DrawTexture {
    TEXTURE_FINAL_IMAGE,
    TEXTURE_VOXEL_ALBEDO,
    TEXTURE_VOXEL_NORMAL,
    TEXTURE_VOXEL_COUNT = TEXTURE_VOXEL_NORMAL,
    TEXTURE_GBUFFER_DEPTH,
    TEXTURE_GBUFFER_ALBEDO,
    TEXTURE_GBUFFER_NORMAL,
    TEXTURE_GBUFFER_METALLIC,
    TEXTURE_GBUFFER_ROUGHNESS,
    TEXTURE_GBUFFER_SHADOW,
    TEXTURE_SSAO,
    TEXTURE_MAX = TEXTURE_SSAO,
};
