#version 450 core
layout (location = 0) in vec3 pass_position;
layout (location = 1) in vec3 pass_normal;

layout (location = 0) out vec4 out_color;

const vec3 lightPos = vec3(0.0, 100.0, 2.0);

void main()
{
    vec3 N = normalize(pass_normal);
    vec3 L = normalize(lightPos - pass_position);

    float ambient = 0.2;
    float diffuse = max(dot(N, L), 0.0);

    out_color = vec4(vec3(ambient + diffuse), 1.0);
}
