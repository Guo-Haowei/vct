#include "LightComponent.h"

#include "engine/Archive.h"

void LightComponent::Serialize(Archive& archive)
{
    (void)archive;
    // if (archive.IsWriteMode())
    // {
    //     archive << type;
    //     archive << color;
    //     archive << energy;
    // }
    // else
    // {
    //     archive >> type;
    //     archive >> color;
    //     archive >> energy;
    // }
}
