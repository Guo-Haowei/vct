layout( location = 0 ) in vec2 pass_uv;
layout( location = 1 ) flat in int pass_id;

layout( location = 0 ) out vec4 out_color;

#include "cbuffer.glsl"

void main()
{
    if ( pass_id == 0 ) {
        out_color = vec4( texture( FinalImage, pass_uv ).rgba );
    }
    else if ( pass_id == 1 ) {
        out_color = vec4( vec3( texture( ShadowMap, pass_uv ).r ), 1.0 );
    }
    else if ( pass_id == 2 ) {
        out_color = vec4( vec3( texture( GbufferDepthMap, pass_uv ).r ), 1.0 );
    }
    else if ( pass_id == 3 ) {
        out_color = vec4( texture( GbufferAlbedoMap, pass_uv ).rgba );
    }
    else if ( pass_id == 4 ) {
        vec3 normal = texture( GbufferNormalRoughnessMap, pass_uv ).rbg;
        normal = 0.5 * ( normal + vec3( 1.0 ) );
        out_color = vec4( normal, 1.0 );
    }
    else if ( pass_id == 5 ) {
        out_color = vec4( vec3( texture( SSAOMap, pass_uv ).r ), 1.0 );
    }
    else if ( pass_id == 6 ) {
        out_color = vec4( vec3( texture( GbufferNormalRoughnessMap, pass_uv ).a ), 1.0 );
    }
    else if ( pass_id == 7 ) {
        out_color = vec4( vec3( texture( GbufferPositionMetallicMap, pass_uv ).a ), 1.0 );
    }
    else {
        discard;
    }
}
