#include "EditorLayer.h"

class Editor : public Application
{
public:
    Editor() : Application(Application::InitInfo{ "Editor", 1920, 1080, false })
    {
        AddLayer(std::make_shared<EditorLayer>());
    }
};

int main(int argc, const char** argv)
{
    Editor editor;
    return editor.Run(argc, argv);
}
