layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in vec3 pass_positions[];
in vec3 pass_normals[];
in vec2 pass_uvs[];
in vec4 pass_light_space_positions0[];
in vec4 pass_light_space_positions1[];
in vec4 pass_light_space_positions2[];

#include "cbuffer.glsl.h"

out vec3 pass_position;
out vec3 pass_normal;
out vec2 pass_uv;
out vec4 pass_light_space_positions[NUM_CASCADES];

void main()
{
    vec3 triangle_normal =
        abs( pass_normals[0] + pass_normals[1] + pass_normals[2] );

    uint dominant = triangle_normal.x > triangle_normal.y ? 0 : 1;
    dominant      = triangle_normal.z > dominant ? 2 : dominant;

    vec3 output_positions[3];

    for ( uint i = 0; i < 3; ++i )
    {
        output_positions[i] = ( pass_positions[i] - c_world_center ) / c_world_size_half;
        if ( dominant == 0 )
        {
            output_positions[i].xyz = output_positions[i].zyx;
        }
        else if ( dominant == 1 )
        {
            output_positions[i].xyz = output_positions[i].xzy;
        }
    }

    // stretch the triangle to fill more one more texel
    vec2 side0N = normalize( output_positions[1].xy - output_positions[0].xy );
    vec2 side1N = normalize( output_positions[2].xy - output_positions[1].xy );
    vec2 side2N = normalize( output_positions[0].xy - output_positions[2].xy );

    output_positions[0].xy += normalize( side2N - side0N ) * c_texel_size;
    output_positions[1].xy += normalize( side0N - side1N ) * c_texel_size;
    output_positions[2].xy += normalize( side1N - side2N ) * c_texel_size;

    for ( uint i = 0; i < 3; ++i )
    {
        pass_position                 = pass_positions[i];
        pass_normal                   = pass_normals[i];
        pass_uv                       = pass_uvs[i];
        pass_light_space_positions[0] = pass_light_space_positions0[i];
        pass_light_space_positions[1] = pass_light_space_positions1[i];
        pass_light_space_positions[2] = pass_light_space_positions2[i];
        gl_Position                   = vec4( output_positions[i].xy, 1.0, 1.0 );
        EmitVertex();
    }
    EndPrimitive();
}
