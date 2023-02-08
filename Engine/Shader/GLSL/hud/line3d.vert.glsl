layout( location = 0 ) in vec3 in_position;
layout( location = 1 ) in vec3 in_color;

#include "cbuffer.shader.hpp"

void main()
{
    vec4 position = vec4( in_position, 1.0 );
    position = Proj * View * Model * position;
    gl_Position = position;
    pass_color = in_color;
}
