#pragma once
#include <string>
#include <list>

using CommandLine = std::list<std::string>;

class Application
{
public:
    bool Run(int argc, const char** argv);

private:
    bool ProcessCmdLine();

    CommandLine mCommandLine;
};
