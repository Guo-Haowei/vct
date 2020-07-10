#pragma once

namespace vct {

constexpr unsigned int log2(unsigned int x)
{
    return x == 1 ? 0 : 1 + log2(x >> 1);
}

constexpr bool isPowerOf2(unsigned int x)
{
    return (x & (x - 1)) == 0;
}

constexpr unsigned int SHADOW_MAP_RESOLUTION = 2048u;

static_assert(isPowerOf2(SHADOW_MAP_RESOLUTION));

constexpr int ALBEDO_VOXEL_SLOT = 0;
constexpr int NORMAL_VOXEL_SLOT = 1;
constexpr int EARLY_Z_SLOT = 2;
constexpr int SHADOW_MAP_SLOT = 3;
constexpr int ALBEDO_MAP_SLOT = 4;
constexpr int NORMAL_MAP_SLOT = 6;
constexpr int METALLIC_ROUGHNESS_SLOT = 7;

constexpr int GBUFFER_ALBEDO_SLOT = 10;
constexpr int GBUFFER_NORMAL_ROUGHNESS_SLOT = 11;
constexpr int GBUFFER_POSITION_METALLIC_SLOT = 12;
constexpr int GBUFFER_AO_SLOT = 13;

constexpr unsigned int VOXEL_TEXTURE_SIZE = 256;
// constexpr unsigned int VOXEL_TEXTURE_SIZE = 128;
constexpr unsigned int VOXEL_TEXTURE_MIP_LEVEL = log2(VOXEL_TEXTURE_SIZE);

constexpr int UNIFORM_BUFFER_CAMERA_SLOT = 0;
constexpr int UNIFORM_BUFFER_LIGHT_SLOT = 1;
constexpr int UNIFORM_BUFFER_MATERIAL_SLOT = 2;

static_assert(VOXEL_TEXTURE_SIZE <= 256);

enum RenderStrategy
{
    NoGI,
    VCT,
    VoxelAlbedo,
    VoxelNormal,
};

enum DrawTexture
{
    TEXTURE_NO_GI,
    TEXTURE_VOXEL_ALBEDO,
    TEXTURE_VOXEL_NORMAL,
    TEXTURE_GBUFFER_NONE,
    TEXTURE_GBUFFER_ALBEDO,
    TEXTURE_GBUFFER_NORMAL,
    TEXTURE_GBUFFER_METALLIC,
    TEXTURE_GBUFFER_ROUGHNESS,
    TEXTURE_GBUFFER_DEPTH,
    TEXTURE_GBUFFER_SHADOW,
};

struct UIControlls
{
    int     voxelMipLevel               = 0;
    int     showVoxelTexture            = -1;
    bool    showObjectBoundingBox       = false;
    bool    showWorldBoundingBox        = false;
    bool    forceUpdateVoxelTexture     = false;
    bool    debugFramebuffers           = true;

    int     objectOccluded              = 0;
    int     totalMaterials              = 0;
    int     totalMeshes                 = 0;
    int     drawTexture                 = TEXTURE_NO_GI;
};

extern UIControlls g_UIControls;

} // namespace vct
