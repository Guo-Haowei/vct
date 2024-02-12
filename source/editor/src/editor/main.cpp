#include "editor_layer.h"

class Editor : public vct::Application {
public:
    void init_layers() override {
        add_layer(std::make_shared<vct::EditorLayer>());
    }
};

int main(int argc, const char** argv) {
    Editor editor;
    return editor.run(argc, argv);
}
