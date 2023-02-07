#pragma once
#include <ostream>

enum class GfxBackend {
    OpenGL,
    D3d11,
};

struct Configuration {
    Configuration( GfxBackend gfxBackend, const char* appName, int w, int h )
        : m_gfxBackend( gfxBackend ), m_appName( appName )
    {
        m_width = w;
        m_height = h;
    }

    const GfxBackend m_gfxBackend;
    const char* m_appName;
    int m_width;
    int m_height;

    bool m_resizable{ true };
    bool m_frameless{ false };
};

inline std::ostream& operator<<( std::ostream& os, const GfxBackend& backend )
{
    switch ( backend ) {
        case GfxBackend::OpenGL:
            os << "OpenGL";
            break;
        case GfxBackend::D3d11:
            os << "Direct3D11";
            break;
    }
    return os;
}

inline std::ostream& operator<<( std::ostream& os, const Configuration& config )
{
    os << "Configuration:";
    os << "\n\tGraphics Backend : " << config.m_gfxBackend;
    os << "\n\tApp Name         : " << config.m_appName;
    os << "\n\tWidth            : " << config.m_width;
    os << "\n\tHeight           : " << config.m_height;
    return os;
}
