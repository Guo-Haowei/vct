#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb/stb_image_write.h"

#pragma pack( push, 1 )
struct RGB8 {
    unsigned char r, g, b;
};
#pragma pack( pop )

static_assert( sizeof( RGB8 ) == 3 * sizeof( unsigned char ) );

#ifdef assert
#undef assert
#endif
#define assert( expr )                                                             \
    if ( !( expr ) )                                                               \
    {                                                                              \
        printf( "%s failed on line %d, in file %s\n", #expr, __LINE__, __FILE__ ); \
        exit( 1 );                                                                 \
    }

int main( int argc, const char** argv )
{
    assert( argc == 2 );
    const char* path = argv[1];

    int width, height, channel;
    RGB8* in = reinterpret_cast<RGB8*>( stbi_load( path, &width, &height, &channel, sizeof( RGB8 ) ) );
    assert( in );

    const int width_half  = width / 2;
    const int height_half = height / 2;
    assert( width_half * 2 == width );
    assert( height_half * 2 == height );

    RGB8* out = new RGB8[width_half * height_half];

    for ( int y = 0; y < height_half; ++y )
    {
        for ( int x = 0; x < width_half; ++x )
        {
            const int x2  = 2 * x;
            const int y2  = 2 * y;
            const RGB8& a = in[y2 * width + x2];
            const RGB8& b = in[y2 * width + x2 + 1];
            const RGB8& c = in[( y2 + 1 ) * width + x2];
            const RGB8& d = in[( y2 + 1 ) * width + x2 + 1];

            RGB8& o = out[y * width_half + x];
            int ir  = a.r + b.r + c.r + d.r;
            int ig  = a.g + b.g + c.g + d.g;
            int ib  = a.b + b.b + c.b + d.b;
            o.r     = static_cast<unsigned char>( ir / 4.0f );
            o.g     = static_cast<unsigned char>( ig / 4.0f );
            o.b     = static_cast<unsigned char>( ib / 4.0f );
        }
    }

    stbi_write_png( "out.png", width_half, height_half, sizeof( RGB8 ), out, width_half * sizeof( RGB8 ) );
}
