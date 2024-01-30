#include <engine/Archive.h>
#include <engine/scene/SceneComponents.h>

class ArchiveTest : public testing::Test {
public:
    static constexpr const char* FILE_NAME = "archive.bin";
    static constexpr const char* TEST_STRING = "add3to2";
    static constexpr char TEST_BYTE = 0x11;
    static constexpr double TEST_DOUBLE = 3.1415926;
};

TEST_F(ArchiveTest, Serialize) {
    Archive writer;

    EXPECT_TRUE(writer.OpenWrite(FILE_NAME));
    EXPECT_TRUE(writer.IsWriteMode());

    writer << TEST_BYTE;
    writer << TEST_STRING;
    writer << TEST_DOUBLE;

    writer.Close();
}

TEST_F(ArchiveTest, Deserialize) {
    Archive reader;
    EXPECT_TRUE(reader.OpenRead(FILE_NAME));
    EXPECT_TRUE(!reader.IsWriteMode());

    char actualByte;
    reader >> actualByte;
    EXPECT_EQ(actualByte, TEST_BYTE);

    std::string actualString;
    reader >> actualString;
    EXPECT_EQ(actualString, std::string(TEST_STRING));

    double actualDouble;
    reader >> actualDouble;
    EXPECT_EQ(actualDouble, TEST_DOUBLE);
    reader.Close();
}

// TEST(Archive, TagComponent)
// {
//     const std::string name = "MyAwesomeObject##001";
//     const char* fileName = "archive2.bin";

//     TagComponent tagComponent(name.c_str());
//     Archive writer;
//     writer.OpenWrite(fileName);
//     tagComponent.Serialize(writer);
//     writer.Close();

//     EXPECT_EQ(tagComponent.GetTag(), name);

//     tagComponent.SetTag("");
//     EXPECT_NE(tagComponent.GetTag(), name);

//     Archive reader;
//     reader.OpenRead(fileName);
//     tagComponent.Serialize(reader);
//     reader.Close();

//     EXPECT_EQ(tagComponent.GetTag(), name);
// }
