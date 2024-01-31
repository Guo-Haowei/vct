#include "LightComponent.h"

#include "Archive.h"

void LightComponent::Serialize(Archive& archive) {
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
