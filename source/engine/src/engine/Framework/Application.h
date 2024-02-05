#pragma once
#include "Layer.h"
#include "ManagerBase.h"

class UIManager;
class WindowManager;

class Application {
public:
    struct InitInfo {
        const char* title;
        bool frameless;
    };

    Application(const InitInfo& info) : mInitInfo(info) {}

    int Run(int argc, const char** argv);
    const InitInfo& GetInfo() const { return mInitInfo; }

protected:
    void AddLayer(std::shared_ptr<Layer> layer);

private:
    void RegisterManager(ManagerBase* manager);
    bool RegisterManagers();
    bool InitializeManagers();
    void FinalizeManagers();

    InitInfo mInitInfo;
    std::vector<ManagerBase*> mManagers;
    std::vector<std::shared_ptr<Layer>> mLayers;
};
