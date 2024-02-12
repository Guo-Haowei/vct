layout(location = 0) in vec4 pass_color;
layout(location = 0) out vec4 out_color;

#include "cbuffer.glsl.h"

void main() {
    if (c_debug_texture_id == 1)  // abledo
    {
        float gamma = 2.2;
        vec3 color = pass_color.rgb;
        color = color / (color + 1.0);
        color = pow(color, vec3(gamma));
        out_color.rgb = color;
    } else  // normal
    {
        out_color.rgb = 0.5 * (pass_color.xyz + 1.0);
    }
}
