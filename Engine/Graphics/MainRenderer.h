#pragma once
#include <memory>

#include "GpuTexture.h"

void createGpuResources();
void destroyGpuResources();

extern GpuTexture m_albedoVoxel;
extern GpuTexture m_normalVoxel;
