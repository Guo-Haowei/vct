#pragma once
#include "Core/BaseManager.hpp"

#include "PipelineStateManager.hpp"
#include "FrameStructure.hpp"

struct GLFWwindow;

class GraphicsManager : _Inherits_ BaseManager {
public:
    GraphicsManager( const char* debugName )
        : BaseManager( debugName ) {}

    virtual void Tick() override;

    virtual void SetPipelineState( const std::shared_ptr<PipelineState>& ) {}

    virtual void DrawBatch( const Frame& ) = 0;

    virtual void InitializeGeometries( const Scene& ) {}

    virtual void BeginFrame( Frame& ) {}
    virtual void EndFrame( Frame& ) {}

private:
    // @TODO: make private
public:
    void CalculateCameraMatrix();
    void CalculateLights();

    void UpdateConstants();

protected:
    Frame m_frame;
};

extern GraphicsManager* g_gfxMgr;
