#pragma once
#include "file_access_unix.h"

namespace vct {

FileAccessUnix::~FileAccessUnix() { close(); }

ErrorCode FileAccessUnix::open_internal(const std::string& path, int mode_flags) {
    ERR_FAIL_COND_V(m_file_handle, ERR_FILE_ALREADY_IN_USE);

    const char* mode = "";
    switch (mode_flags) {
        case READ:
            mode = "rb";
            break;
        case WRITE:
            mode = "wb";
            break;
        default:
            return ERR_INVALID_PARAMETER;
    }

    m_file_handle = fopen(path.c_str(), mode);

    if (!m_file_handle) {
        switch (errno) {
            case ENOENT:
                return ERR_FILE_NOT_FOUND;
            default:
                return ERR_FILE_CANT_OPEN;
        }
    }

    return OK;
}

void FileAccessUnix::close() {
    if (m_file_handle) {
        fclose(m_file_handle);
        m_file_handle = nullptr;
    }
}

bool FileAccessUnix::is_open() const { return m_file_handle != nullptr; }

size_t FileAccessUnix::get_length() const {
    CRASH_NOW_MSG("TODO");
    return 0;
}

bool FileAccessUnix::read_buffer(void* data, size_t size) const {
    DEV_ASSERT(is_open());

    if (fread(data, 1, size, m_file_handle) != size) {
        return false;
    }

    return true;
}

bool FileAccessUnix::write_buffer(const void* data, size_t size) {
    DEV_ASSERT(is_open());

    if (fwrite(data, 1, size, m_file_handle) != size) {
        return false;
    }

    return true;
}

}  // namespace vct
