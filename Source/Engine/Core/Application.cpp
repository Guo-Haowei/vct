#include "Application.h"

#include "CommonDvars.h"
#include "Log.h"

// @TODO: refactor
#include "com_filesystem.h"
#include "lua_script.h"

bool Application::Run(int argc, const char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        mCommandLine.push_back(argv[i]);
    }

    if (!Com_FsInit())
    {
        return false;
    }

    if (!ProcessCmdLine())
    {
        return false;
    }

    return true;
}

static void register_common_dvars()
{
#define REGISTER_DVAR
#include "CommonDvars.h"
}

class CommandHelper
{
public:
    CommandHelper(const CommandLine& cmdLine) : mCommandLine(cmdLine)
    {
    }

    bool TryConsume(std::string& str)
    {
        if (mCommandLine.empty())
        {
            str.clear();
            return false;
        }

        str = mCommandLine.front();
        mCommandLine.pop_front();
        return true;
    }

    bool Consume(std::string& str)
    {
        if (mCommandLine.empty())
        {
            LOG_ERROR("Unexpected EOF");
            str.clear();
            return false;
        }

        return TryConsume(str);
    }

private:
    CommandLine mCommandLine;
};

bool Application::ProcessCmdLine()
{
    register_common_dvars();

    CommandHelper cmdHelper(mCommandLine);

    std::string arg;
    while (cmdHelper.TryConsume(arg))
    {
        if (arg == "+set")
        {
            cmdHelper.Consume(arg);
            DynamicVariable* dvar = DynamicVariableManager::Find(arg.c_str());
            if (dvar == nullptr)
            {
                LOG_ERROR("[dvar] Dvar '{}' not found", arg);
                return false;
            }
            cmdHelper.Consume(arg);
            dvar->SetFromSourceString(arg.c_str());
        }
        else if (arg == "+exec")
        {
            cmdHelper.Consume(arg);
            if (!Com_ExecLua(arg.c_str()))
            {
                LOG_ERROR("[lua] failed to execute script '{}'", arg);
                return false;
            }
        }
        else
        {
            LOG_ERROR("Unknown command '{}'", arg);
            return false;
        }
    }

    LOG_OK("cmd line processed");

    return true;
}
