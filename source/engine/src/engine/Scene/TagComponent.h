#pragma once

namespace vct {
class Archive;
}
using namespace vct;

class TagComponent {
public:
    TagComponent() = default;

    TagComponent(const char* tag) { mTag = tag; }

    // bool operator==(const std::string& tag) const { return mTag == tag; }

    void Serialize(Archive& archive);

    void SetTag(const char* tag) { mTag = tag; }
    void SetTag(const std::string& tag) { mTag = tag; }

    const std::string& GetTag() const { return mTag; }
    std::string& GetTagRef() { return mTag; }

private:
    std::string mTag;
};
