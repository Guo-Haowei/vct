#pragma once
#include <memory>

#include "GpuTexture.h"
#include "gl_utils.h"
#include "r_cbuffers.h"
#include "r_rendertarget.h"

class MainRenderer {
public:
    void createGpuResources();
    void destroyGpuResources();

private:
    /// vertex arrays
    std::shared_ptr<MeshData> m_box;  // no normals

    /// textures
};

extern MainRenderer renderer;
extern GpuTexture m_albedoVoxel;
extern GpuTexture m_normalVoxel;
