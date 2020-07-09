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
constexpr int SPECULAR_MAP_SLOT = 5;
constexpr int NORMAL_MAP_SLOT = 6;
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

struct UIControlls
{
    int renderStrategy              = VCT;
    int voxelMipLevel               = 0;
    int showVoxelTexture            = -1;
    bool showObjectBoundingBox      = false;
    bool showWorldBoundingBox       = false;
    bool forceUpdateVoxelTexture    = false;
    bool showDepthBuffers           = true;
    int objectOccluded              = 0;
};

extern UIControlls g_UIControls;

} // namespace vct
