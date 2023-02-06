layout(location = 0) in vec3 pass_color;
layout(location = 0) out vec4 out_color;

void main() {
  vec3 color = pass_color;
  out_color = vec4(color, 1.0);
}
