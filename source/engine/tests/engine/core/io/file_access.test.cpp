#include "core/io/file_access_unix.h"

namespace vct {

static std::string s_buffer;
class FileAccessFoo : public FileAccessUnix {
public:
    virtual ErrorCode open_internal(const std::string& path, int mode_flags) override {
        auto info = std::format("[open]f:{},m:{},a:{};", path, mode_flags, (int)get_access_type());
        s_buffer.append(info);
        return OK;
    }
};

TEST(file_access, make_default) {
    FileAccess::make_default<FileAccessFoo>(FileAccess::ACCESS_RESOURCE);
    FileAccess::make_default<FileAccessFoo>(FileAccess::ACCESS_USERDATA);
    FileAccess::make_default<FileAccessFoo>(FileAccess::ACCESS_FILESYSTEM);

    { auto file = FileAccess::open("a.txt", 10); }
    EXPECT_EQ(s_buffer, "[open]f:a.txt,m:10,a:2;");
    { auto file = FileAccess::open("res://abc.txt", 1); }
    EXPECT_EQ(s_buffer, "[open]f:a.txt,m:10,a:2;[open]f:res://abc.txt,m:1,a:0;");
    { auto file = FileAccess::open("user://cache", 7); }
    EXPECT_EQ(s_buffer, "[open]f:a.txt,m:10,a:2;[open]f:res://abc.txt,m:1,a:0;[open]f:user://cache,m:7,a:1;");

    s_buffer.clear();
}

}  // namespace vct
