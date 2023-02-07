#pragma once
#include <vector>
#include "Interface/IApplication.hpp"
#include "Interface/IGraphicsManager.hpp"
#include "Interface/IPipelineStateManager.hpp"

#include "Manager/AssetLoader.hpp"

class BaseApplication : public IApplication {
public:
    BaseApplication() = default;
    ~BaseApplication() override = default;
    virtual bool Initialize();
    virtual void Finalize();

    virtual void Tick();

    virtual bool ProcessCommandLineParameters( int argc, const char** argv ) override;

    bool IsQuit() const override;
    void RequestQuit() override { m_bQuit = true; }

    void GetFramebufferSize( uint32_t&, uint32_t& ) override {}

    void CreateMainWindow() override {}
    void* GetMainWindowHandler() override { return nullptr; }

    void RegisterManagerModule( IGraphicsManager* mgr );
    void RegisterManagerModule( IPipelineStateManager* mgr );
    void RegisterManagerModule( FileManager* mgr );

    IGraphicsManager* GetGraphicsManager()
    {
        return m_pGraphicsManager;
    }
    IPipelineStateManager* GetPipelineStateManager()
    {
        return m_pPipelineStateManager;
    }

protected:
    // Flag if need quit the main loop of the application
    bool m_bQuit = false;

    IGraphicsManager* m_pGraphicsManager = nullptr;
    IPipelineStateManager* m_pPipelineStateManager = nullptr;
    FileManager* m_pAssetLoader = nullptr;

private:
    std::vector<IRuntimeModule*> m_runtimeModules;
};