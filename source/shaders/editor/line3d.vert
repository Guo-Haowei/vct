layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec3 pass_color;

#include "cbuffer.glsl"

void main() {
  vec4 position = vec4(in_position, 1.0);
  position = PVM * position;
  gl_Position = position;
  pass_color = in_color;
}
