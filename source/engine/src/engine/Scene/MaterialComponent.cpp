#include "MaterialComponent.h"

#include "Core/Archive.h"

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
