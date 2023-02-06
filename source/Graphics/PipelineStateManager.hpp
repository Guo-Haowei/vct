#pragma once
#include <map>
#include <memory>
#include <string>

#include "Interface/IPipelineStateManager.hpp"

class PipelineStateManager : public IPipelineStateManager {
public:
    ~PipelineStateManager() override;

    virtual bool Initialize() override;
    virtual void Finalize() override;
    virtual void Tick() override {}

    virtual const std::shared_ptr<PipelineState> GetPipelineState( const std::string& name ) const override;

    bool RegisterPipelineState( PipelineState& pipelineState );
    void UnregisterPipelineState( PipelineState& pipelineState );
    void Clear();

protected:
    virtual bool InitializePipelineState( PipelineState** )
    {
        return true;
    }
    virtual void DestroyPipelineState( PipelineState& ) {}

protected:
    std::map<std::string, std::shared_ptr<PipelineState>> m_pipelineStates;
};
