#pragma once
#include "Base/Defines.h"

#include "shaders/cbuffer.glsl"

#include <vector>
#include <memory>

class Entity;

struct DrawFrameContext : PerFrameConstants {
};

struct DrawBatchContext : PerBatchConstants {
    int32_t batchIndex{ 0 };

    const Entity* pEntity;
    // material_textures material;

    virtual ~DrawBatchContext() = default;
};

struct Frame {
    DrawFrameContext frameContexts;
    std::vector<std::shared_ptr<DrawBatchContext>> batchContexts;
};