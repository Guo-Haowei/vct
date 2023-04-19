#pragma once
#include <list>
#include <string>
#include <vector>

#include "ManagerBase.h"

class UIManager;
class WindowManager;

using CommandLine = std::list<std::string>;

class Application
{
public:
    bool Run(int argc, const char** argv);

private:
    bool ProcessCmdLine();
    bool RegisterManagers();
    bool InitializeManagers();
    void FinalizeManagers();

    CommandLine mCommandLine;
    std::vector<ManagerBase*> mManagers;
};
