#include "MaterialComponent.h"

#include "engine/Archive.h"

void MaterialComponent::Serialize(Archive& archive)
{
    if (archive.IsWriteMode())
    {
        archive << mMetallic;
        archive << mRoughness;
        archive << mBaseColor;
    }
    else
    {
        archive >> mMetallic;
        archive >> mRoughness;
        archive >> mBaseColor;
    }
}
