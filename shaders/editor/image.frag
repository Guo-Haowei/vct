layout(location = 0) in vec2 pass_uv;
layout(location = 0) out vec4 out_color;

uniform sampler2D texture0;

void main() {
  vec4 color = texture(texture0, pass_uv);
  if (color.a < 0.01) {
    discard;
  }
  out_color = color;
}
