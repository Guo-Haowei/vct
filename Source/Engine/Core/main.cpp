#include "Application.h"

int main(int argc, const char** argv)
{
    Application app;
    return app.Run(argc, argv) ? 0 : 1;
}
