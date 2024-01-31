#pragma once
#include "file_access.h"

namespace vct {

class FileAccessUnix : public FileAccess {
public:
    ~FileAccessUnix();

    virtual void close() override;
    virtual bool is_open() const override;
    virtual size_t get_length() const override;
    virtual bool read_buffer(void* data, size_t size) const override;
    virtual bool write_buffer(const void* data, size_t size) override;

protected:
    virtual ErrorCode open_internal(const std::string& path, int mode_flags) override;

    FILE* m_file_handle{ nullptr };
};

}  // namespace vct
