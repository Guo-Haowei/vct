layout( location = 0 ) in vec2 in_position;

layout( location = 0 ) out vec2 pass_uv;

#include "cbuffer.glsl"

void main()
{
    vec2 translate = OverlayPositions[0].xy;
    vec2 scale = OverlayPositions[0].zw;
    vec2 point = in_position * scale + translate;
    gl_Position = vec4( point, 0.0, 1.0 );
    pass_uv = 0.5 * ( in_position + 1.0 );
}
