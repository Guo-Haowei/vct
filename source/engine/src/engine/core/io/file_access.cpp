#include "file_access.h"

namespace vct {

FileAccess::CreateFunc FileAccess::s_create_func[ACCESS_COUNT];

auto FileAccess::create(AccessType access_type) -> std::shared_ptr<FileAccess> {
    DEV_ASSERT_INDEX(access_type, ACCESS_COUNT);

    auto ret = s_create_func[access_type]();
    ret->set_access_type(access_type);
    return std::shared_ptr<FileAccess>(ret);
}

auto FileAccess::create_for_path(const std::string& path) -> std::shared_ptr<FileAccess> {
    // @TODO: sanitize path
    if (path.starts_with("res://")) {
        return create(ACCESS_RESOURCE);
    }

    if (path.starts_with("user://")) {
        return create(ACCESS_USERDATA);
    }

    return create(ACCESS_FILESYSTEM);
}

auto FileAccess::open(const std::string& path, int mode_flags)
    -> std::expected<std::shared_ptr<FileAccess>, Error<ErrorCode>> {
    auto file_access = create_for_path(path);

    ErrorCode err = file_access->open_internal(path, mode_flags);
    if (err != OK) {
        return VCT_ERROR(err, "failed to open file '{}'.", path);
    }

    return file_access;
}

}  // namespace vct
