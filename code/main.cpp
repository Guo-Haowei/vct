#include "App.h"
#include <Windows.h>

// force NV card selection
extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int main()
{
    g_pApp->run();
    return 0;
}
