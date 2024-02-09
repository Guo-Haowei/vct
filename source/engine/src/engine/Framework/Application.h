#pragma once
#include "Layer.h"
#include "ManagerBase.h"

class UIManager;

class Application {
public:
    int Run(int argc, const char** argv);

protected:
    void AddLayer(std::shared_ptr<Layer> layer);

private:
    std::vector<std::shared_ptr<Layer>> mLayers;
};
