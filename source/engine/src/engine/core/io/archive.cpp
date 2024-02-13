#include "archive.h"

#include "core/io/print.h"

namespace vct {

auto Archive::open_mode(const std::string& path, bool write_mode) -> std::expected<void, Error<ErrorCode>> {
    m_path = path;
    m_write_mode = write_mode;
    if (m_write_mode) {
        m_path += ".tmp";
    }

    auto res = FileAccess::open(m_path, write_mode ? FileAccess::WRITE : FileAccess::READ);
    if (!res) {
        return std::unexpected(res.error());
    }

    m_file = *res;
    return std::expected<void, Error<ErrorCode>>();
}

void Archive::close() {
    if (!m_file) {
        return;
    }

    m_file.reset();

    if (m_write_mode) {
        std::filesystem::path final_path{ m_path };
        final_path.replace_extension();
        std::filesystem::rename(m_path, final_path);
    }
}

bool Archive::is_write_mode() const {
    DEV_ASSERT(m_file);
    return m_write_mode;
}

bool Archive::write(const void* data, size_t size) {
    DEV_ASSERT(m_file && m_write_mode);
    return m_file->write_buffer(data, size);
}

bool Archive::read(void* data, size_t size) {
    DEV_ASSERT(m_file && !m_write_mode);
    return m_file->read_buffer(data, size);
}

Archive& Archive::write_string(const char* data, size_t length) {
    write(length);
    write(data, length);
    return *this;
}

}  // namespace vct