layout( location = 0 ) in vec2 pass_uv;

layout( location = 0 ) out vec4 out_color;

#include "cbuffer.glsl"

void main()
{
    out_color = vec4( vec3( texture( ShadowMap, pass_uv ).r ), 1.0 );
}
