#pragma once
#include "Core/GraphicsManager.h"

[[nodiscard]] bool R_Init();

class GLGraphicsManager : public GraphicsManager {
public:
    GLGraphicsManager()
        : GraphicsManager( "GLGraphicsManager" )
    {
    }

    virtual bool Init() override;
    virtual void Deinit() override;
};
