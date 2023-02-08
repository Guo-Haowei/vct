#pragma once
#include <memory>

#include "GpuTexture.h"
#include "r_cbuffers.h"
#include "r_rendertarget.h"

void createGpuResources();
void destroyGpuResources();

extern GpuTexture m_albedoVoxel;
extern GpuTexture m_normalVoxel;
