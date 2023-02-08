layout( location = 0 ) in vec3 in_position;

#include "cbuffer.shader.hpp"

void main()
{
    gl_Position = LightPV * Model * vec4( in_position, 1.0 );
}
