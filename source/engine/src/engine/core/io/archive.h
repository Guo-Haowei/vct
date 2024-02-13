#pragma once
#include "file_access.h"

namespace vct {

class Archive {
public:
    ~Archive() {
        close();
    }

    [[nodiscard]] auto open_read(const std::string& path) { return open_mode(path.c_str(), false); }
    [[nodiscard]] auto open_write(const std::string& path) { return open_mode(path.c_str(), true); }

    void close();
    bool is_write_mode() const;

    Archive& operator<<(const char* value) {
        return write_string(value, strlen(value));
    }

    Archive& operator<<(const std::string& value) {
        return write_string(value.data(), value.length());
    }

    Archive& operator>>(std::string& value) {
        size_t stringLength = 0;
        read(stringLength);
        value.resize(stringLength);
        read(value.data(), stringLength);
        return *this;
    }

    template<typename T, class = typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
    Archive& operator<<(const std::vector<T>& value) {
        uint64_t size = value.size();
        write(size);
        write(value.data(), sizeof(T) * size);
        return *this;
    }

    template<typename T, class = typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
    Archive& operator>>(std::vector<T>& value) {
        uint64_t size = 0;
        read(size);
        value.resize(size);
        read(value.data(), sizeof(T) * size);
        return *this;
    }

    template<typename T, class = typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
    Archive& operator<<(const T& value) {
        write(&value, sizeof(T));
        return *this;
    }

    template<typename T, class = typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
    Archive& operator>>(T& value) {
        read(&value, sizeof(T));
        return *this;
    }

    template<typename T, class = typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
    bool write(const T& value) {
        return write(&value, sizeof(value));
    }

    template<typename T, class = typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
    bool read(T& value) {
        return read(&value, sizeof(value));
    }

    bool write(const void* data, size_t size);
    bool read(void* data, size_t size);

private:
    [[nodiscard]] auto open_mode(const std::string& path, bool write_mode) -> std::expected<void, Error<ErrorCode>>;

    Archive& write_string(const char* data, size_t length);

    bool m_write_mode{ false };
    std::shared_ptr<FileAccess> m_file;
    std::string m_path;
};

}  // namespace vct
