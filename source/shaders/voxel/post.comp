layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;
layout( rgba16f, binding = 0 ) uniform image3D u_albedo_texture;
layout( rgba16f, binding = 1 ) uniform image3D u_normal_texture;

void main()
{
    int voxel_texture_size = imageSize( u_albedo_texture ).x;
    uint global_index_1D =
        gl_GlobalInvocationID.x * gl_NumWorkGroups.y * gl_NumWorkGroups.z +
        gl_GlobalInvocationID.y * gl_NumWorkGroups.z + gl_GlobalInvocationID.z;
    int coord_x = int( global_index_1D ) % voxel_texture_size;
    int coord_y = ( int( global_index_1D ) / voxel_texture_size ) % voxel_texture_size;
    int coord_z = int( global_index_1D ) / ( voxel_texture_size * voxel_texture_size );

    ivec3 coord = ivec3( coord_x, coord_y, coord_z );
    vec4 color  = imageLoad( u_albedo_texture, coord );
    vec4 normal = imageLoad( u_normal_texture, coord );
    if ( color.a < 0.0001 )
        return;

    color /= color.a;
    normal /= normal.a;
    imageStore( u_albedo_texture, coord, color );
    imageStore( u_normal_texture, coord, normal );
}
