#pragma once

namespace vct {

constexpr unsigned int log2(unsigned int x)
{
    return x == 1 ? 0 : 1 + log2(x >> 1);
}

constexpr int ALBEDO_VOXEL_SLOT = 0;
constexpr int NORMAL_VOXEL_SLOT = 1;
constexpr unsigned int VOXEL_TEXTURE_SIZE = 64;
constexpr unsigned int VOXEL_TEXTURE_MIP_LEVEL = log2(VOXEL_TEXTURE_SIZE);

struct UIControlls
{
    int renderStrategy              = 0;
    int voxelMipLevel               = 0;
    bool showObjectBoundingBox      = false;
    bool showWorldBoundingBox       = false;
    bool forceUpdateVoxelTexture    = false;
};

extern UIControlls g_UIControls;

} // namespace vct
