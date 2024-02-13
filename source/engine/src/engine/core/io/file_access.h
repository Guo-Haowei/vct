#pragma once

namespace vct {

class FileAccess {
public:
    using CreateFunc = FileAccess* (*)(void);

    enum AccessType {
        ACCESS_FILESYSTEM,
        ACCESS_RESOURCE,
        ACCESS_USERDATA,
        ACCESS_MAX,
    };

    enum ModeFlags {
        NONE = 0,
        READ = 1,
        WRITE = 2,
    };

    virtual ~FileAccess() = default;

    virtual void close() = 0;
    virtual bool is_open() const = 0;
    virtual size_t get_length() const = 0;

    virtual bool read_buffer(void* data, size_t size) const = 0;
    virtual bool write_buffer(const void* data, size_t size) = 0;

    AccessType get_access_type() const { return m_access_type; }

    static auto create(AccessType access_type) -> std::shared_ptr<FileAccess>;
    static auto create_for_path(const std::string& path) -> std::shared_ptr<FileAccess>;
    // @TODO: use string_view
    static auto open(const std::string& path, int mode_flags)
        -> std::expected<std::shared_ptr<FileAccess>, Error<ErrorCode>>;

    template<typename T>
    static void make_default(AccessType access_type) {
        s_create_func[access_type] = create_builtin<T>;
    }

protected:
    FileAccess() = default;

    virtual ErrorCode open_internal(std::string_view path, int mode_flags) = 0;
    virtual void set_access_type(AccessType access_type) { m_access_type = access_type; }
    virtual std::string fix_path(std::string_view path);

    AccessType m_access_type = ACCESS_MAX;

    static CreateFunc s_create_func[ACCESS_MAX];

private:
    template<typename T>
    static FileAccess* create_builtin() {
        return new T;
    }
};

}  // namespace vct
