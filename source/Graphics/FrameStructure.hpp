#pragma once
#include "Core/scene.h"

#include "Base/Defines.h"

#define REGISTER( a )

#include "shaders/cbuffer.glsl"

// @TODO: fix
//using Texture2D = uint32_t;

// struct material_textures {
//     Texture2D diffuseMap;
//     Texture2D normalMap;
//     Texture2D metallicMap;
//     Texture2D roughnessMap;
//     Texture2D aoMap;
//     Texture2D heightMap;
// };

struct DrawBatchContext : PerBatchConstants {
    int32_t batchIndex{ 0 };

    const Geometry* pGeom;
    // material_textures material;

    virtual ~DrawBatchContext() = default;
};

struct Frame {
    std::vector<std::shared_ptr<DrawBatchContext>> batchContexts;
};