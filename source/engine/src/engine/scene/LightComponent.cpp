#include "LightComponent.h"

#include "core/io/archive.h"

void LightComponent::Serialize(Archive& archive) {
    (void)archive;
    // if (archive.is_write_mode())
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
