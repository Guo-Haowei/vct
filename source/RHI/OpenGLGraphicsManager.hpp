#pragma once
#include "Graphics/GraphicsManager.hpp"

// @TODO: rename to OpenGL
class OpenGLGraphicsManager : public GraphicsManager {
public:
    virtual bool Initialize() override;
    virtual void Finalize() override;

    virtual void SetPipelineState( const std::shared_ptr<PipelineState>& pipelineState ) final;

    virtual void DrawBatch( const Frame& frame ) final;

    virtual void InitializeGeometries( const Scene& scene ) final;

    virtual void BeginFrame( Frame& frame ) final;
    virtual void EndFrame( Frame& frame ) final;

    virtual void Draw() final;

protected:
    void SetPerFrameConstants( const DrawFrameContext& context );
    void SetPerBatchConstants( const DrawBatchContext& context );

    struct GLDrawBatchContext : public DrawBatchContext {
        uint32_t vao{ 0 };
        uint32_t mode{ 0 };
        uint32_t type{ 0 };
        int32_t count{ 0 };
    };

    uint32_t m_uboDrawFrameConstant = { 0 };
    uint32_t m_uboDrawBatchConstant = { 0 };
};
