#include "menu_bar.h"

#include <imgui/imgui.h>

#include "core/framework/common_dvars.h"
#include "core/framework/scene_manager.h"
#include "platform/windows/dialog.h"

namespace vct {

// @TODO: fix this
static std::vector<std::string> quick_dirty_split(std::string str, std::string token) {
    std::vector<std::string> result;
    while (str.size()) {
        size_t index = str.find(token);
        if (index != std::string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0) result.push_back(str);
        } else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

static void import_scene(ImporterName importer) {
    std::vector<const char*> filters = { ".gltf" };
    auto path = open_file_dialog(filters);

    if (path.empty()) {
        return;
    }

    SceneManager::singleton().request_scene(path, importer);

    std::string files(DVAR_GET_STRING(recent_files));
    if (!files.empty()) {
        files.append(";");
    }
    files.append(path);

    DVAR_SET_STRING(recent_files, files);
}

static void import_recent(ImporterName importer) {
    std::string recent_files(DVAR_GET_STRING(recent_files));

    auto files = quick_dirty_split(recent_files, ";");

    for (const auto& file : files) {
        if (ImGui::MenuItem(file.c_str())) {
            SceneManager::singleton().request_scene(file, importer);
        }
    }
    ImGui::EndMenu();
}

static void save_project(bool open_dialog) {
    const std::string& project = DVAR_GET_STRING(project);

    std::filesystem::path path{ project.empty() ? "untitled.scene" : project.c_str() };
    if (open_dialog || project.empty()) {
        if (!open_save_dialog(path)) {
            return;
        }
    }

    DVAR_SET_STRING(project, path.string());
    Scene& scene = SceneManager::singleton().get_scene();

    Archive archive;
    if (!archive.open_write(path.string())) {
        return;
    }

    scene.serialize(archive);

    LOG("scene saved to '{}'", path.string());
}

void menu_bar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                save_project(false);
            }
            if (ImGui::MenuItem("Save As..")) {
                save_project(true);
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Import")) {
            if (ImGui::MenuItem("Import (Assimp)")) {
                import_scene(IMPORTER_ASSIMP);
            }
            if (ImGui::BeginMenu("Import Recent (Assimp)")) {
                import_recent(IMPORTER_ASSIMP);
            }
            if (ImGui::MenuItem("Import (TinyGLTF)")) {
                import_scene(IMPORTER_TINYGLTF);
            }
            if (ImGui::BeginMenu("Import Recent (TinyGLTF)")) {
                import_recent(IMPORTER_TINYGLTF);
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {
            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {
            }  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {
            }
            if (ImGui::MenuItem("copy", "CTRL+C")) {
            }
            if (ImGui::MenuItem("Paste", "CTRL+V")) {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

}  // namespace vct
