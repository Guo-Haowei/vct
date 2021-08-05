#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "universal/core_math.h"

struct MeshComponent {
    enum Flag {
        None               = 0,
        HAS_UV_FLAG        = 1 << 0,
        HAS_NORMAL_FLAG    = 1 << 1,
        HAS_TANGENT_FLAG   = 1 << 2,
        HAS_BITANGENT_FLAG = 1 << 3,
    };

    uint32_t flags = None;
    std::string name;
    std::vector<vec3> positions;
    std::vector<vec2> uvs;
    std::vector<vec3> normals;
    std::vector<vec3> tangents;
    std::vector<vec3> bitangents;
    std::vector<uint32_t> indices;

    uint32_t materialIdx = static_cast<uint32_t>( -1 );
};
