#pragma once
#include <engine/container/fixed_string.h>

class Archive;

class TagComponent
{
public:
    using TagString = fixed_string<256>;

    TagComponent() = default;

    TagComponent(const char* tag)
    {
        mTag = tag;
    }

    //bool operator==(const std::string& tag) const { return mTag == tag; }

    void Serialize(Archive& archive);

    void SetTag(const char* tag) { mTag = tag; }
    void SetTag(const std::string& tag) { mTag = tag; }

    const TagString& GetTag() const { return mTag; }
    TagString& GetTagRef() { return mTag; }

private:
    TagString mTag;
};
