#include "dialog.h"

#include <Windows.h>
#include <commdlg.h>

namespace vct {

std::string open_file_dialog(const std::vector<const char*>& filters) {
    std::string filterStr;
    if (filters.empty()) {
        filterStr = "*.*";
    } else {
        for (const auto& filter : filters) {
            filterStr.append(";*");
            filterStr.append(filter);
        }
        filterStr = filterStr.substr(1);
    }

    char buf[1024] = { 0 };
    snprintf(buf, sizeof(buf), "Supported Files(%s)\n%s", filterStr.c_str(), filterStr.c_str());
    for (char* p = buf; *p; ++p) {
        if (*p == '\n') {
            *p = '\0';
            break;
        }
    }

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    char szFile[260] = { 0 };
    ofn.lStructSize = sizeof(ofn);
    // ofn.hwndOwner = DEV_ASSERT(0);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = buf;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameA(&ofn)) {
        return std::string(szFile);
    }

    return "";
}

template<size_t N>
static void copy_string(char (&buffer)[N], const std::string& string) {
    strncpy(buffer, string.c_str(), N);
}

bool open_save_dialog(std::filesystem::path& inout_path) {
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    // @TODO: string_view
    char file_name[MAX_PATH]{ 0 };
    char extension[MAX_PATH]{ 0 };
    char dir[MAX_PATH]{ 0 };
    copy_string(file_name, inout_path.filename().replace_extension().string());
    copy_string(dir, inout_path.parent_path().string());
    copy_string(extension, inout_path.extension().string());

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    // ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = file_name;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = extension + 1;
    ofn.lpstrInitialDir = dir;

    if (GetSaveFileNameA(&ofn)) {
        inout_path = std::filesystem::path(ofn.lpstrFile);
        return true;
    }

    return false;
}

}  // namespace vct
