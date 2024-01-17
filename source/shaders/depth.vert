layout( location = 0 ) in vec3 in_position;

#include "cbuffer.glsl"

void main()
{
    gl_Position = PVM * vec4( in_position, 1.0 );
}
