#include "MaterialComponent.h"

#include "core/io/archive.h"

void MaterialComponent::Serialize(Archive& archive) {
    if (archive.is_write_mode()) {
        archive << mMetallic;
        archive << mRoughness;
        archive << mBaseColor;
    } else {
        archive >> mMetallic;
        archive >> mRoughness;
        archive >> mBaseColor;
    }
}
