#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "Core/Archive.h"
#include "Scene/SceneComponents.h"

TEST_CASE("Archive")
{
    const std::string fileName = "archive.bin";
    SUBCASE("Serialize")
    {
        const std::string testString = "add3to2";
        const char testByte = 0x11;
        const double testDouble = 3.1415926;
        {
            Archive writer;

            CHECK(writer.OpenWrite(fileName));
            CHECK(writer.IsWriteMode());

            writer << testByte;
            writer << testString;
            writer << testDouble;

            writer.Close();
        }

        {
            Archive reader;
            CHECK(reader.OpenRead(fileName));
            CHECK(!reader.IsWriteMode());

            char testByteReadBack;
            reader >> testByteReadBack;
            CHECK(testByte == testByteReadBack);

            std::string testStringReadBack;
            reader >> testStringReadBack;
            CHECK(testStringReadBack == testString);

            double testDoubleReadBack;
            reader >> testDoubleReadBack;
            CHECK(testDouble == testDoubleReadBack);

            reader.Close();
        }
    }

    SUBCASE("Components")
    {
        TagComponent::TagString name("Object1");
        {
            TagComponent TagComponent(name.c_str());
            Archive writer;
            writer.OpenWrite(fileName);
            TagComponent.Serialize(writer);
            writer.Close();
        }

        {
            TagComponent TagComponent;
            Archive reader;
            reader.OpenRead(fileName);
            TagComponent.Serialize(reader);
            CHECK(TagComponent.GetTag() == name);
            reader.Close();
        }
    }
}
