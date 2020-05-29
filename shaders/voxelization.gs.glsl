#version 450 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 pass_positions[];
in vec3 pass_normals[];

out vec3 pass_position; // fragment world position
out vec3 pass_normal; // fragment normal

uniform vec3 u_world_center;
uniform vec3 u_world_size; // half of the world size

void main(){
	const vec3 p0 = pass_positions[1] - pass_positions[0];
	const vec3 p1 = pass_positions[2] - pass_positions[0];
	const vec3 triangle_normal = abs(cross(p0, p1)); 
    uint dominant = triangle_normal.x > triangle_normal.y ? 0 : 1;
    dominant = triangle_normal.z > dominant ? 2 : dominant;
    for(uint i = 0; i < 3; ++i)
    {
        // transform gl_Position from world space to ndc space
        pass_position = gl_Position.xyz;
        pass_normal = pass_normals[i];
        gl_Position = vec4((pass_positions[i] - u_world_center) / u_world_size, 1.0);
        if (dominant == 0)
        {
            gl_Position.xyz = gl_Position.zyx;
        }
        else if (dominant == 1)
        {
            gl_Position.xyz = gl_Position.xzy;
        }
        // projected position

        // TODO: expand vertex?
        EmitVertex();
    }
    EndPrimitive();
}
