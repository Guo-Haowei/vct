#pragma once
#include <vector>
#include "Interface/IApplication.hpp"
#include "Interface/IAssetLoader.hpp"
#include "Interface/IGraphicsManager.hpp"
#include "Interface/IPipelineStateManager.hpp"
#include "Interface/ISceneManager.hpp"

class BaseApplication : public IApplication {
public:
    BaseApplication() = default;
    ~BaseApplication() override = default;

    virtual bool Initialize( int argc, const char** argv ) override;
    virtual void Finalize();

    virtual void Tick();

    bool IsQuit() const override;
    void RequestQuit() override { m_bQuit = true; }

    void GetFramebufferSize( uint32_t&, uint32_t& ) override {}

    void CreateMainWindow() override {}
    void* GetMainWindowHandler() override { return nullptr; }

    void RegisterManagerModule( IAssetLoader* mgr );
    void RegisterManagerModule( ISceneManager* mgr );
    void RegisterManagerModule( IGraphicsManager* mgr );
    void RegisterManagerModule( IPipelineStateManager* mgr );

    IAssetLoader* GetAssetLoader()
    {
        return m_pAssetLoader;
    }

    ISceneManager* GetSceneManager()
    {
        return m_pSceneManager;
    }

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

    IAssetLoader* m_pAssetLoader = nullptr;
    ISceneManager* m_pSceneManager = nullptr;
    IGraphicsManager* m_pGraphicsManager = nullptr;
    IPipelineStateManager* m_pPipelineStateManager = nullptr;

private:
    std::vector<IRuntimeModule*> m_runtimeModules;
};