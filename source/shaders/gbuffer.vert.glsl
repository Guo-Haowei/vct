#include "cbuffer.glsl"

layout( location = 0 ) in vec3 in_position;
layout( location = 1 ) in vec3 in_normal;
layout( location = 2 ) in vec2 in_uv;
layout( location = 3 ) in vec3 in_tangent;
layout( location = 4 ) in vec3 in_bitangent;

out struct PS_INPUT {
    vec3 position;
    vec2 uv;
    mat3 TBN;
} ps_in;

void main()
{
    vec4 world_position = M * vec4( in_position, 1.0 );
    ps_in.position = world_position.xyz;
    mat3 rotation = mat3( M );
    vec3 T = normalize( rotation * in_tangent );
    vec3 B = normalize( rotation * in_bitangent );
    vec3 N = normalize( rotation * in_normal );

    gl_Position = PV * world_position;
    ps_in.uv = in_uv;
    ps_in.TBN = mat3( T, B, N );
}
