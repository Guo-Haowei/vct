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

    return std::string();
}
}  // namespace vct
