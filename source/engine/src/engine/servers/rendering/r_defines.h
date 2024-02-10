#pragma once
#include "Core/Defines.h"

inline constexpr int IMAGE_VOXEL_ALBEDO_SLOT = 0;
inline constexpr int IMAGE_VOXEL_NORMAL_SLOT = 1;

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
