#pragma once
#include "Graphics/GraphicsManager.hpp"

struct GLFWwindow;

class OpenGLGraphicsManager : public GraphicsManager {
public:
    virtual bool Initialize() override;
    virtual void Finalize() override;

    virtual void SetPipelineState( const std::shared_ptr<PipelineState>& pipelineState ) override;

    virtual void DrawBatch( const Frame& frame ) override;

    virtual void InitializeGeometries( const Scene& scene ) override;

    virtual void BeginFrame( Frame& frame ) override;
    virtual void EndFrame( Frame& frame ) override;

    virtual void Draw() override;
    virtual void Present() override;

protected:
    void SetPerFrameConstants( const DrawFrameContext& context );
    void SetPerBatchConstants( const DrawBatchContext& context );

    // @TODO: rename this to OpenGLDrawBatchContext
    struct GLDrawBatchContext : public DrawBatchContext {
        uint32_t vao{ 0 };
        uint32_t mode{ 0 };
        uint32_t type{ 0 };
        int32_t count{ 0 };
    };

    uint32_t m_uboDrawFrameConstant = { 0 };
    uint32_t m_uboDrawBatchConstant = { 0 };

    GLFWwindow* m_pGlfwWindow;
};
