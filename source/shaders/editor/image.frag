layout(location = 0) in vec2 pass_uv;
layout(location = 0) out vec4 out_color;

#include "cbuffer.glsl.h"

void main() {
    vec4 color = texture(LightIconTextures[0], pass_uv);
    if (color.a < 0.01) {
        discard;
    }
    out_color = color;
}
