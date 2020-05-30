#version 450 core
in vec3 pass_position;
in vec3 pass_normal;
layout (location = 0) out vec4 out_color;

void main()
{
    vec3 lightPos = vec3(0, 2000, 3);
    float diffuse = max(0, dot(normalize(pass_normal), normalize(lightPos - pass_position)));
    vec3 color = (diffuse) * vec3(0.8);
    out_color = vec4(color, 1.0);
}
