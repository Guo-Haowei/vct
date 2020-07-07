#pragma once

namespace vct {

struct UIControlls
{
    int renderStrategy              = 0;
    bool showObjectBoundingBox      = false;
    bool showWorldBoundingBox       = false;
    bool forceUpdateVoxelTexture    = false;
};

extern UIControlls g_UIControls;

} // namespace vct
