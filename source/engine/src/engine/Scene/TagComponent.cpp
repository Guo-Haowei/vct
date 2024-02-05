#include "TagComponent.h"

#include "core/io/archive.h"

void TagComponent::Serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << mTag;
    } else {
        archive >> mTag;
    }
}
