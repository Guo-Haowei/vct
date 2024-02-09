#include "menu_bar.h"

#include <imgui/imgui.h>

#include "core/dynamic_variable/common_dvars.h"
#include "platform/windows/dialog.h"
#include "scene/scene_manager.h"

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

void menu_bar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                // LoadScene();
                std::vector<const char*> filters = { ".gltf" };
                auto path = vct::open_file_dialog(filters);

                if (!path.empty()) {
                    vct::SceneManager::singleton().request_scene(path);

                    std::string files(DVAR_GET_STRING(recent_files));
                    if (!files.empty()) {
                        files.append(";");
                    }
                    files.append(path);

                    DVAR_SET_STRING(recent_files, files);
                }
            }
            if (ImGui::BeginMenu("Open Recent")) {
                std::string recent_files(DVAR_GET_STRING(recent_files));

                auto files = quick_dirty_split(recent_files, ";");

                for (const auto& file : files) {
                    if (ImGui::MenuItem(file.c_str())) {
                        vct::SceneManager::singleton().request_scene(file);
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
            }
            if (ImGui::MenuItem("Save As..")) {
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
            if (ImGui::MenuItem("Copy", "CTRL+C")) {
            }
            if (ImGui::MenuItem("Paste", "CTRL+V")) {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}
