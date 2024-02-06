#pragma once

namespace vct {
class Archive;
}
using namespace vct;

struct LightComponent {
    // LIGHT_TYPE type = LIGHT_TYPE_NONE;
    // vec3 color = vec3(1);
    // float energy = 10.0f;

    void Serialize(Archive& archive);
};
