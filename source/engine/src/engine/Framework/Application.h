#pragma once
#include "Layer.h"
#include "ManagerBase.h"

class UIManager;
class WindowManager;

using CommandLine = std::list<std::string>;

class Application {
public:
    struct InitInfo {
        const char* title;
        uint32_t width;
        uint32_t height;
        bool frameless;
    };

    Application(const InitInfo& info) : mInitInfo(info) {}

    int Run(int argc, const char** argv);
    const InitInfo& GetInfo() const { return mInitInfo; }

protected:
    void AddLayer(std::shared_ptr<Layer> layer);

private:
    bool ProcessCmdLine();
    void RegisterManager(ManagerBase* manager);
    bool RegisterManagers();
    bool InitializeManagers();
    void FinalizeManagers();

    InitInfo mInitInfo;
    CommandLine mCommandLine;
    std::vector<ManagerBase*> mManagers;
    std::vector<std::shared_ptr<Layer>> mLayers;
};
