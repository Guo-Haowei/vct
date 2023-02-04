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

    virtual void DrawBatch( const Frame& frame ) override;

private:
    void SetPerBatchConstants( const DrawBatchContext& context );

    virtual void InitializeGeometries( const Scene& scene ) final;

    struct GLDrawBatchContext : public DrawBatchContext {
        uint32_t vao{ 0 };
        uint32_t mode{ 0 };
        uint32_t type{ 0 };
        int32_t count{ 0 };
    };

    // @TODO: make multiple frames
    uint32_t m_uboDrawBatchConstant = { 0 };
};
