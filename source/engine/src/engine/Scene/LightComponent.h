#pragma once

class Archive;

struct LightComponent {
    // LIGHT_TYPE type = LIGHT_TYPE_NONE;
    // vec3 color = vec3(1);
    // float energy = 10.0f;

    void Serialize(Archive& archive);
};
