#version 450 core
in vec3 pass_position;
in vec3 pass_normal;
in vec2 pass_uv;
layout (location = 0) out vec4 out_color;

uniform sampler2D u_albedo;

void main()
{
    // vec3 lightPos = vec3(0, 2000, 3);
    // float diffuse = max(0, dot(normalize(pass_normal), normalize(lightPos - pass_position)));
    vec3 lightDir = normalize(vec3(0, 1, 1));
    float diffuse = max(0, dot(normalize(pass_normal), lightDir));
    vec4 color = (diffuse) * texture(u_albedo, pass_uv);
    out_color = vec4(color);
}
