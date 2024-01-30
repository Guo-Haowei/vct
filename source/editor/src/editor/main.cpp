#include "EditorLayer.h"

static constexpr int WIDTH = 1920;
static constexpr int HEIGHT = 1080;

class Editor : public Application {
public:
    Editor() : Application(Application::InitInfo{ "Editor", WIDTH, HEIGHT, false }) {
        AddLayer(std::make_shared<EditorLayer>());
    }
};

int main(int argc, const char** argv) {
    Editor editor;
    return editor.Run(argc, argv);
}
