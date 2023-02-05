#pragma once
#include "Core/scene.h"

#include "Base/Defines.h"

#define REGISTER( a )

#include "shaders/cbuffer.glsl"

struct DrawFrameContext : PerFrameConstants {
};

struct DrawBatchContext : PerBatchConstants {
    int32_t batchIndex{ 0 };

    const Geometry* pGeom;
    // material_textures material;

    virtual ~DrawBatchContext() = default;
};

struct Frame {
    DrawFrameContext frameContexts;
    std::vector<std::shared_ptr<DrawBatchContext>> batchContexts;
};