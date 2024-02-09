#include "cbuffer.glsl.h"
#include "vsinput.glsl.h"

void main() {
    gl_Position = c_projection_view_model_matrix * vec4(in_position, 1.0);
}
