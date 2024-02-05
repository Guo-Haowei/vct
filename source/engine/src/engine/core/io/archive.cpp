#include "archive.h"

#include "core/io/print.h"

namespace vct {

auto Archive::open_mode(const std::string& path, bool write_mode) -> std::expected<void, Error<ErrorCode>> {
    auto res = FileAccess::open(path, write_mode ? FileAccess::WRITE : FileAccess::READ);
    if (!res) {
        return std::unexpected(res.error());
    }

    m_file = *res;
    m_write_mode = write_mode;
    return std::expected<void, Error<ErrorCode>>();
}

void Archive::close() {
    m_file.reset();
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