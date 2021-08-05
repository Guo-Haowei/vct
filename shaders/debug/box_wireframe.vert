layout(location = 0) in vec3 in_position;

layout(std140, binding = 0) uniform Camera { mat4 PV; };

uniform vec3 u_center;
uniform vec3 u_size;

void main() {
  vec3 position = in_position * u_size + u_center;
  vec4 world_position = vec4(position, 1.0);
  gl_Position = PV * world_position;
}
