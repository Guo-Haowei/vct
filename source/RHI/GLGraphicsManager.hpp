#pragma once
#include "Graphics/GraphicsManager.hpp"

[[nodiscard]] bool R_Init();

class GLGraphicsManager : public GraphicsManager {
public:
    GLGraphicsManager()
        : GraphicsManager( "GLGraphicsManager" )
    {
    }

    virtual bool Init() override;
    virtual void Deinit() override;

    virtual void SetPipelineState( const std::shared_ptr<PipelineState>& pipelineState ) override;
};
