#pragma once
#include "Manager/GraphicsManager.hpp"

struct OpenGLMeshData {
    uint32_t vao{ 0 };
    uint32_t ebo{ 0 };
    uint32_t vbos[5]{ 0 };
    uint32_t count{ 0 };
};

class OpenGLGraphicsManager : public GraphicsManager {
public:
    virtual bool Initialize() override;
    virtual void Finalize() override;

    virtual void SetPipelineState( const std::shared_ptr<PipelineState>& pipeline_state ) override;

    virtual void DrawBatch( const Frame& frame ) override;

    virtual void InitializeGeometries( const Scene& scene ) override;

    virtual void BeginFrame( Frame& frame ) override;
    virtual void EndFrame( Frame& frame ) override;

    virtual void Present() override;

protected:
    void SetPerFrameConstants( const DrawFrameContext& context );
    void SetPerBatchConstants( const DrawBatchContext& context );

    bool SetShaderParameter( const char* param_name, const int32_t value );

    struct OpenGLDrawBatchContext : public DrawBatchContext {
        uint32_t vao{ 0 };
        uint32_t mode{ 0 };
        uint32_t type{ 0 };
        int32_t count{ 0 };
    };

    uint32_t m_uboDrawFrameConstant{ 0 };
    uint32_t m_uboDrawBatchConstant{ 0 };

    uint32_t m_currentShader;
    std::vector<std::shared_ptr<OpenGLMeshData>> m_sceneMeshData;
};

extern OpenGLMeshData g_quad;