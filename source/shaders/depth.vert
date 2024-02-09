#include "cbuffer.glsl.h"

#define INPUT_POS
#include "common/vsinput.glsl"

void main() {
    gl_Position = c_projection_view_model_matrix * vec4(in_position, 1.0);
}
