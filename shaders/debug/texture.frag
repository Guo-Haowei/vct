layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 pass_uv;

uniform sampler2D u_texture;
uniform int u_type;

void main() {
  if (u_type == 0) // depth
    out_color = vec4(texture(u_texture, pass_uv).rrr, 1.0);
  else if (u_type == 1) // albedo, position
    out_color = vec4(texture(u_texture, pass_uv).rgb, 1.0);
  else if (u_type == 2) // normal
    out_color = vec4(0.5 * texture(u_texture, pass_uv).rgb + 0.5, 1.0);
  else if (u_type == 3) // roughness metallic
    out_color = vec4(texture(u_texture, pass_uv).aaa, 1.0);
  else
    return;
}
