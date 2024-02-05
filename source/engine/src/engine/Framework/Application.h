#pragma once
#include "Layer.h"
#include "ManagerBase.h"

class UIManager;
class WindowManager;

class Application {
public:
    int Run(int argc, const char** argv);

protected:
    void AddLayer(std::shared_ptr<Layer> layer);

private:
    void RegisterManager(ManagerBase* manager);
    bool RegisterManagers();
    bool InitializeManagers();
    void FinalizeManagers();

    std::vector<ManagerBase*> mManagers;
    std::vector<std::shared_ptr<Layer>> mLayers;
};
