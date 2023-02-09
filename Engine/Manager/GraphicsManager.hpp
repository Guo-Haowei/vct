#pragma once
#include "Interface/IGraphicsManager.hpp"

#include "SceneGraph/Scene.hpp"

#include "DrawPass/ShadowMapPass.hpp"
#include "DrawPass/ForwardPass.hpp"
#include "DrawPass/DeferredPass.hpp"
#include "DrawPass/OverlayPass.hpp"
#include "DrawPass/VoxelizationPass.hpp"
#include "DrawPass/GuiPass.hpp"

struct GLFWwindow;

class GraphicsManager : public IGraphicsManager {
public:
    virtual bool Initialize() override;
    virtual void Finalize() override;

    virtual void Tick() override;

    virtual void Draw() override;
    virtual void Present() override {}

    virtual void ResizeCanvas( int, int ) override {}

    virtual void SetPipelineState( const std::shared_ptr<PipelineState>& ) {}

    virtual void DrawBatch( const Frame& ) = 0;

    virtual void BeginPass( Frame& ) override {}
    virtual void EndPass( Frame& ) override {}

    virtual void ReleaseTexture( TextureBase& ) override {}

protected:
    virtual void BeginScene( const Scene& scene );
    virtual void EndScene();

    virtual void BeginFrame( Frame& ) {}
    virtual void EndFrame( Frame& ) {}

    // @TODO: make protected:
    virtual void InitializeGeometries( const Scene& ) {}

private:
    void CalculateCameraMatrix();
    void CalculateLights();

    void UpdateConstants();

protected:
    Frame m_frame;
    uint64_t m_nSceneRevision{0};
    bool m_bInitialized{ false };
    std::vector<std::shared_ptr<BaseDrawPass>> m_drawPasses;
    std::vector<std::shared_ptr<MaterialTextures>> m_sceneTextures;
};
