#pragma once
#include "Interface/IGraphicsManager.hpp"

#include "FrameStructure.hpp"
#include "DrawPass/BaseDrawPass.hpp"
#include "SceneGraph/Scene.hpp"

struct GLFWwindow;

class GraphicsManager : public IGraphicsManager {
public:
    virtual bool Initialize() override;
    virtual void Tick() override;

    virtual void Draw() override;
    virtual void Present() override {}

    virtual void SetPipelineState( const std::shared_ptr<PipelineState>& ) {}

    virtual void DrawBatch( const Frame& ) = 0;

    virtual void BeginFrame( Frame& ) {}
    virtual void EndFrame( Frame& ) {}

    virtual void BeginPass( Frame& ) override {}
    virtual void EndPass( Frame& ) override {}

public:
    // @TODO: make protected:
    virtual void InitializeGeometries( const Scene& ) {}

private:
    void CalculateCameraMatrix();
    void CalculateLights();

    void UpdateConstants();

protected:
    Frame m_frame;
    bool m_bInitialized{ false };
    std::vector<std::shared_ptr<BaseDrawPass>> m_drawPasses;
};

extern GraphicsManager* g_gfxMgr;
