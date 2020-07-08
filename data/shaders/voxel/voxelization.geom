#version 450 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 pass_positions[];
in vec3 pass_normals[];
in vec2 pass_uvs[];

out vec3 pass_position; // fragment world position
out vec3 pass_normal; // fragment normal
out vec2 pass_uv;

uniform vec4 u_world; // xyz : world center; w : world size
uniform int u_voxel_texture_size;

void main(){
	const vec3 p0 = pass_positions[1] - pass_positions[0];
	const vec3 p1 = pass_positions[2] - pass_positions[0];
	const vec3 triangle_normal = abs(cross(p0, p1));
    uint dominant = triangle_normal.x > triangle_normal.y ? 0 : 1;
    dominant = triangle_normal.z > dominant ? 2 : dominant;

    vec3 output_positions[3];
    // vec3 output_normals[3];
    for(uint i = 0; i < 3; ++i)
    {
        // transform gl_Position from world space to ndc space
        // output_normals[i] = pass_normals[i];
        output_positions[i] = (pass_positions[i] - u_world.xyz) / (0.5 * u_world.w);
        if (dominant == 0)
        {
            output_positions[i].xyz = output_positions[i].zyx;
        }
        else if (dominant == 1)
        {
            output_positions[i].xyz = output_positions[i].xzy;
        }
        // gl_Position = vec4(output_positions[i], 1.0);
        // EmitVertex();
    }
    // stretch the triangle to fill more texels
    vec2 side0N = normalize(output_positions[1].xy - output_positions[0].xy);
    vec2 side1N = normalize(output_positions[2].xy - output_positions[1].xy);
    vec2 side2N = normalize(output_positions[0].xy - output_positions[2].xy);
    const float texelSize = 2.0 / float(u_voxel_texture_size);
    output_positions[0].xy += normalize(side2N - side0N) * texelSize;
    output_positions[1].xy += normalize(side0N - side1N) * texelSize;
    output_positions[2].xy += normalize(side1N - side2N) * texelSize;

    for (uint i = 0; i < 3; ++i)
    {
        pass_position = pass_positions[i];
        pass_normal = pass_normals[i];
        pass_uv = pass_uvs[i];
        gl_Position = vec4(output_positions[i], 1.0);
        EmitVertex();
    }
    EndPrimitive();
}
