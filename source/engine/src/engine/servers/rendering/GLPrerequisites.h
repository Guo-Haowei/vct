#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <glad/glad.h>

#include "assets/image.h"

namespace vct {

inline GLuint format_to_gl_format(PixelFormat format) {
    switch (format) {
        case FORMAT_R8_UINT:
        case FORMAT_R16_FLOAT:
        case FORMAT_R32_FLOAT:
            return GL_RED;
        case FORMAT_R8G8_UINT:
        case FORMAT_R16G16_FLOAT:
        case FORMAT_R32G32_FLOAT:
            return GL_RG;
        case FORMAT_R8G8B8_UINT:
        case FORMAT_R16G16B16_FLOAT:
        case FORMAT_R32G32B32_FLOAT:
            return GL_RGB;
        case FORMAT_R8G8B8A8_UINT:
        case FORMAT_R16G16B16A16_FLOAT:
        case FORMAT_R32G32B32A32_FLOAT:
            return GL_RGBA;
        case FORMAT_D32_FLOAT:
            return GL_DEPTH_COMPONENT;
        default:
            CRASH_NOW();
            return 0;
    }
}

inline GLuint format_to_gl_internal_format(PixelFormat format) {
    switch (format) {
        case FORMAT_R8_UINT:
            return GL_RED;
        case FORMAT_R8G8_UINT:
            return GL_RG;
        case FORMAT_R8G8B8_UINT:
            return GL_RGB;
        case FORMAT_R8G8B8A8_UINT:
            return GL_RGBA;
        case FORMAT_R16_FLOAT:
            return GL_R16F;
        case FORMAT_R16G16_FLOAT:
            return GL_RG16F;
        case FORMAT_R16G16B16_FLOAT:
            return GL_RGB16F;
        case FORMAT_R16G16B16A16_FLOAT:
            return GL_RGBA16F;
        case FORMAT_R32_FLOAT:
            return GL_R32F;
        case FORMAT_R32G32_FLOAT:
            return GL_RG32F;
        case FORMAT_R32G32B32_FLOAT:
            return GL_RGB32F;
        case FORMAT_R32G32B32A32_FLOAT:
            return GL_RGBA32F;
        case FORMAT_D32_FLOAT:
            return GL_DEPTH_COMPONENT;
        default:
            CRASH_NOW();
            return 0;
    }
}

inline GLuint format_to_gl_data_type(PixelFormat format) {
    switch (format) {
        case FORMAT_R8_UINT:
        case FORMAT_R8G8_UINT:
        case FORMAT_R8G8B8_UINT:
        case FORMAT_R8G8B8A8_UINT:
            return GL_UNSIGNED_BYTE;
        case FORMAT_R16_FLOAT:
        case FORMAT_R16G16_FLOAT:
        case FORMAT_R16G16B16_FLOAT:
        case FORMAT_R16G16B16A16_FLOAT:
        case FORMAT_R32_FLOAT:
        case FORMAT_R32G32_FLOAT:
        case FORMAT_R32G32B32_FLOAT:
        case FORMAT_R32G32B32A32_FLOAT:
        case FORMAT_D32_FLOAT:
            return GL_FLOAT;
        default:
            CRASH_NOW();
            return 0;
    }
}

}  // namespace vct
