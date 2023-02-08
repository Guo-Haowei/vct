#include "Image.hpp"

#include <filesystem>

#include "Base/Logger.h"
#include "Base/StringUtils.hpp"

#include "stb_image.h"

namespace fs = std::filesystem;
using std::string;

void Image::Release()
{
    if ( m_pPixels ) {
        free( m_pPixels );
    }
    MemZero( *this );
}

bool Image::Load( const std::string& sys_path, Image& out_image )
{
    if ( !fs::exists( sys_path ) ) {
        LOG_FATAL( "Image file '%s' not found", sys_path.c_str() );
        return false;
    }

    int width = 0, height = 0, channel = 0;
    uint8_t* pixels = stbi_load( sys_path.c_str(), &width, &height, &channel, 4 );

    if ( !pixels ) {
        LOG_FATAL( "stbi_load(\"'%s'\") failed", sys_path.c_str() );
        return false;
    }

    switch ( channel ) {
        case 3:
            out_image.m_pixelFormat = PIXEL_FORMAT::RGB8;
            break;
        case 4:
            out_image.m_pixelFormat = PIXEL_FORMAT::RGBA8;
            break;
        default:
            LOG_FATAL( "Unsupported channel %d", channel );
            return false;
    }

    out_image.m_pPixels = pixels;
    out_image.m_width = width;
    out_image.m_height = height;
    out_image.m_pixelFormat = PIXEL_FORMAT::RGBA8;
    return true;
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"