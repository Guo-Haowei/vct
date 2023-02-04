#pragma once

#include "Base/IManager.h"

struct GLFWwindow;

class GraphicsManager : public IManager {
public:
    GraphicsManager( const char* debugName )
        : IManager( debugName )
    {
    }
};

extern GraphicsManager* g_gfxMgr;
