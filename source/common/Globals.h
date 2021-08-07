#pragma once

#include "universal/universal.h"

namespace vct {

// constexpr unsigned int VOXEL_TEXTURE_SIZE = 256;
constexpr unsigned int VOXEL_TEXTURE_SIZE      = 128;
constexpr unsigned int VOXEL_TEXTURE_MIP_LEVEL = log2( VOXEL_TEXTURE_SIZE );

static_assert( VOXEL_TEXTURE_SIZE <= 256 );

}  // namespace vct
enum DrawTexture {
    TEXTURE_FINAL_IMAGE,
    TEXTURE_VOXEL_ALBEDO,
    TEXTURE_VOXEL_NORMAL,
    TEXTURE_GBUFFER_NONE,
    TEXTURE_GBUFFER_DEPTH,
    TEXTURE_GBUFFER_ALBEDO,
    TEXTURE_GBUFFER_NORMAL,
    TEXTURE_GBUFFER_METALLIC,
    TEXTURE_GBUFFER_ROUGHNESS,
    // TODO: rename
    TEXTURE_GBUFFER_SHADOW,
};
struct UIControlls {
    int voxelMipLevel    = 0;
    int showVoxelTexture = -1;

    int drawTexture = TEXTURE_FINAL_IMAGE;
    int voxelGiMode = 1;

    bool forceUpdateVoxelTexture = false;
    bool debugFramebuffers       = true;
};
extern UIControlls g_UIControls;

inline constexpr int UNIFORM_BUFFER_VS_PER_FRAME_SLOT = 0;
inline constexpr int UNIFORM_BUFFER_FS_PER_FRAME_SLOT = 1;
inline constexpr int UNIFORM_BUFFER_MATERIAL_SLOT     = 2;
inline constexpr int UNIFORM_BUFFER_CONSTANT_SLOT     = 3;

inline constexpr int IMAGE_VOXEL_ALBEDO_SLOT   = 0;
inline constexpr int IMAGE_VOXEL_NORMAL_SLOT   = 1;
inline constexpr int TEXTURE_VOXEL_ALBEDO_SLOT = 2;
inline constexpr int TEXTURE_VOXEL_NORMAL_SLOT = 3;

inline constexpr int ALBEDO_MAP_SLOT         = 4;
inline constexpr int NORMAL_MAP_SLOT         = 6;
inline constexpr int METALLIC_ROUGHNESS_SLOT = 7;

inline constexpr int TEXTURE_SHADOW_MAP_SLOT                = 3;
inline constexpr int TEXTURE_GBUFFER_DEPTH_SLOT             = 9;
inline constexpr int TEXTURE_GBUFFER_ALBEDO_SLOT            = 10;
inline constexpr int TEXTURE_GBUFFER_NORMAL_ROUGHNESS_SLOT  = 11;
inline constexpr int TEXTURE_GBUFFER_POSITION_METALLIC_SLOT = 12;
inline constexpr int TEXTURE_GBUFFER_AO_SLOT                = 13;
