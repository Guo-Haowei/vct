#include "EditorLayer.h"
#include "core/os/os.h"

static constexpr int WIDTH = 1920;
static constexpr int HEIGHT = 1080;

class Editor : public Application {
public:
    Editor() : Application(Application::InitInfo{ "Editor", WIDTH, HEIGHT, false }) {
        AddLayer(std::make_shared<EditorLayer>());
    }
};

int main(int argc, const char** argv) {
    // @TODO: init os properly
    static vct::OS s_os;
    s_os.add_logger(std::make_shared<vct::StdLogger>());

    Editor editor;
    return editor.Run(argc, argv);
}
