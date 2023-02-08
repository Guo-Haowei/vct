layout( location = 0 ) in vec3 in_position;
layout( location = 1 ) in vec3 in_normal;
layout( location = 2 ) in vec2 in_uv;

#include "cbuffer.shader.hpp"

out vec3 pass_positions;
out vec3 pass_normals;
out vec2 pass_uvs;
out vec4 pass_light_space_positions0;

void main()
{
    // assume no transformation
    vec4 world_position = Model * vec4( in_position, 1.0 );
    pass_positions = world_position.xyz;
    pass_normals = normalize( in_normal );
    pass_uvs = in_uv;
    pass_light_space_positions0 = LightPV * world_position;
    gl_Position = world_position;
}
