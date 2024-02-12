#pragma once
#include "file_access.h"

namespace vct {

class FileAccessUnix : public FileAccess {
public:
    ~FileAccessUnix();

    void close() override;
    bool is_open() const override;
    size_t get_length() const override;
    bool read_buffer(void* data, size_t size) const override;
    bool write_buffer(const void* data, size_t size) override;

protected:
    ErrorCode open_internal(std::string_view path, int mode_flags) override;

    FILE* m_file_handle{ nullptr };
};

}  // namespace vct
