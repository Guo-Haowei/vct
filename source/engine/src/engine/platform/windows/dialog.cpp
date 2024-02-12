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

std::string open_save_dialog(const std::string&) {
    OPENFILENAMEA ofn;

    char szFileName[MAX_PATH] = "untitled";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    // ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    // @TODO: fix
    ofn.lpstrDefExt = "scene";

    if (GetSaveFileNameA(&ofn)) {
        return std::string(ofn.lpstrFile);
    }

    return "";
}

}  // namespace vct
