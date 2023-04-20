#include "Engine/Framework/Application.h"

int main(int argc, const char** argv)
{
    Application::InitInfo info{ "Editor", 1920, 1080, false };
    Application app(info);
    return app.Run(argc, argv);
}
