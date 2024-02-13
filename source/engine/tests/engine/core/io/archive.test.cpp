#include "core/io/archive.h"

#include "core/io/file_access_unix.h"
#include "scene/scene_components.h"

namespace vct {

TEST(Archive, open_read) {
    FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_FILESYSTEM);

    Archive archive;
    auto err = archive.open_read("path_that_does_not_exist").error();
    EXPECT_EQ(err.get_value(), ERR_FILE_NOT_FOUND);
}

TEST(Archive, open_write) {
    const std::string test_file = "archive_test_open_write";

    Archive archive;
    auto result = archive.open_write(test_file);
    EXPECT_TRUE(result);
    EXPECT_TRUE(archive.is_write_mode());

    archive.close();
    EXPECT_TRUE(std::filesystem::remove(test_file));
}

TEST(Archive, write_and_read) {
    const char* test_file = "archive_test_write_and_read";
    const char* test_cstring = "add3to2";
    const std::string test_string = "add3to2";
    const int test_int = 12345;
    const char test_byte = 0x11;
    const double test_double = 3.1415926;

    Archive writer;

    EXPECT_TRUE(writer.open_write(test_file));
    EXPECT_TRUE(writer.is_write_mode());

    writer << test_int;
    writer << test_byte;
    writer << test_string;
    writer << test_double;
    writer << test_cstring;

    writer.close();

    Archive reader;
    EXPECT_TRUE(reader.open_read(test_file));
    EXPECT_FALSE(reader.is_write_mode());

    int actual_int;
    reader >> actual_int;
    EXPECT_EQ(actual_int, test_int);

    char actual_byte;
    reader >> actual_byte;
    EXPECT_EQ(actual_byte, test_byte);

    std::string actual_string;
    reader >> actual_string;
    EXPECT_EQ(actual_string, test_string);

    double actual_double;
    reader >> actual_double;
    EXPECT_EQ(actual_double, test_double);

    std::string actual_c_string;
    reader >> actual_c_string;
    EXPECT_EQ(actual_c_string, std::string(test_cstring));

    reader.close();

    EXPECT_TRUE(std::filesystem::remove(test_file));
}

// @TODO:
// TEST(Archive, serialze_tag_component) {
//     const std::string name = "MyAwesomeObject##001";
//     const std::string TAG_COMPONENT_SERIALIZE_TEST_FILE = "archive_test_serialize_tag_component";
//
//     TagComponent tag_component(name);
//     Archive writer;
//     auto res = writer.open_write(TAG_COMPONENT_SERIALIZE_TEST_FILE);
//     ASSERT_TRUE(res);
//     tag_component.serialize(writer);
//     writer.close();
//
//     EXPECT_EQ(tag_component.get_tag(), name);
//
//     tag_component.set_tag("");
//     EXPECT_NE(tag_component.get_tag(), name);
//
//     Archive reader;
//     res = reader.open_read(TAG_COMPONENT_SERIALIZE_TEST_FILE);
//     ASSERT_TRUE(res);
//
//     tag_component.serialize(reader);
//     reader.close();
//
//     EXPECT_EQ(tag_component.get_tag(), name);
//
//     ASSERT_TRUE(fs::remove(TAG_COMPONENT_SERIALIZE_TEST_FILE));
// }

}  // namespace vct
