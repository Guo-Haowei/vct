layout( location = 0 ) in vec2 in_position;

layout( location = 0 ) out vec2 pass_uv;
layout( location = 1 ) flat out int pass_id;

#include "cbuffer.glsl"

void main()
{
    vec2 translate = OverlayPositions[gl_InstanceID].xy;
    vec2 scale = OverlayPositions[gl_InstanceID].zw;
    vec2 point = in_position * scale + translate;
    gl_Position = vec4( point, 0.0, 1.0 );
    pass_uv = 0.5 * ( in_position + 1.0 );
    pass_id = gl_InstanceID;
}
