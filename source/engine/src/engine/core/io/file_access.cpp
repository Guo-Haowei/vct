#include "file_access.h"

namespace vct {

FileAccess::CreateFunc FileAccess::s_create_func[ACCESS_MAX];

auto FileAccess::create(AccessType access_type) -> std::shared_ptr<FileAccess> {
    DEV_ASSERT_INDEX(access_type, ACCESS_MAX);

    auto ret = s_create_func[access_type]();
    ret->set_access_type(access_type);
    return std::shared_ptr<FileAccess>(ret);
}

auto FileAccess::create_for_path(const std::string& path) -> std::shared_ptr<FileAccess> {
    // @TODO: sanitize path
    if (path.starts_with("@res://")) {
        return create(ACCESS_RESOURCE);
    }

    if (path.starts_with("@user://")) {
        return create(ACCESS_USERDATA);
    }

    return create(ACCESS_FILESYSTEM);
}

auto FileAccess::open(const std::string& path, int mode_flags)
    -> std::expected<std::shared_ptr<FileAccess>, Error<ErrorCode>> {
    auto file_access = create_for_path(path);

    ErrorCode err = file_access->open_internal(file_access->fix_path(path), mode_flags);
    if (err != OK) {
        return VCT_ERROR(err, "error code: {}", std::to_underlying(err));
    }

    return file_access;
}

static void replace_first(std::string& string, std::string_view pattern, std::string_view replacement) {
    string.replace(0, pattern.size(), replacement);
}

std::string FileAccess::fix_path(std::string_view path) {
    std::string fixed_path{ path };
    switch (m_access_type) {
        case ACCESS_RESOURCE: {
            if (path.starts_with("@res://")) {
                replace_first(fixed_path, "@res:/", ROOT_FOLDER "resources");
                return fixed_path;
            }
        } break;
        case ACCESS_USERDATA: {
            if (path.starts_with("@user://")) {
                replace_first(fixed_path, "@user:/", ROOT_FOLDER "user");
                return fixed_path;
            }
        } break;
        default:
            break;
    }
    return fixed_path;
}

}  // namespace vct
