#include "application/Application.h"
#include <Windows.h>
// force NV card selection
extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int main(int argc, const char** argv)
{
    vct::Application app;
    app.commandLineArgs(argc, argv);
    return app.run();
}
