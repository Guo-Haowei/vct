#include "TagComponent.h"
#include "engine/Archive.h"

void TagComponent::Serialize(Archive& archive)
{
    if (archive.IsWriteMode())
    {
        archive << mTag;
    }
    else
    {
        archive >> mTag;
    }
}
