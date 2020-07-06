#include "SceneManager.h"
#include <string>
#include <assert.h>
#ifndef MODEL_DIR
#define MODEL_DIR ""
#endif

int main(int argc, char** argv)
{
    assert(argc > 1);
    std::string name = argv[1];
    std::string folder = std::string(MODEL_DIR) + name;
    std::string file = name + ".obj";
    g_pSceneManager->load(folder.c_str(), file.c_str());
    g_pSceneManager->write();
    return 0;
}
