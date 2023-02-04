#include "com_filesystem.h"

#include <cstring>
#include <filesystem>

#include "universal/dvar_api.h"

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "com_dvars.h"

#define PRINT_FILESYSTEM IN_USE
#if USING( PRINT_FILESYSTEM )
#define FS_PRINT( fmt, ... )      ::base::Log( ::base::Level::Debug, "[filesystem] " fmt, ##__VA_ARGS__ )
#define FS_PRINT_WARN( fmt, ... ) ::base::Log( ::base::Level::Warn, "[filesystem] " fmt, ##__VA_ARGS__ )
#else
#define FS_PRINT( fmt, ... )      ( (void)0 )
#define FS_PRINT_WARN( fmt, ... ) ( (void)0 )
#endif

#ifndef ROOT_FOLDER
#define ROOT_FOLDER ""
#endif

static struct {
    char base[kMaxOSPath];
    bool initialized;
} s_glob;

static void FsReplaceSeparator( char* path )
{
    for ( char* it = path; *it; ++it ) {
        if ( *it == '\\' ) {
            *it = '/';
        }
    }
}

bool Com_FsInit()
{
    if ( s_glob.initialized ) {
        return true;
    }

    // setup basepath
    // @TODO: hard code
    strncpy( s_glob.base, "D:/workspace/VCT/", sizeof( s_glob.base ) );
    FsReplaceSeparator( s_glob.base );

    FS_PRINT( "base path is '%s'", s_glob.base );

    s_glob.initialized = true;
    return true;
}

void Com_FsBuildPath( char* buf, size_t bufLen, const char* filename, const char* path )
{
    ASSERT( s_glob.initialized );
    snprintf( buf, bufLen, "%s%s/%s", s_glob.base, path ? path : "", filename );
}

ComFile Com_FsOpenRead( const char* filename, const char* path )
{
    ASSERT( s_glob.initialized );
    ASSERT( filename );

    char fullpath[kMaxOSPath];
    // try relative
    snprintf( fullpath, kMaxOSPath, "%s/%s", path ? path : ".", filename );
    if ( !std::filesystem::exists( fullpath ) ) {
        Com_FsBuildPath( fullpath, kMaxOSPath, filename, path );
    }

    ComFile file;
    file.handle = std::fopen( fullpath, "r" );
    if ( file.handle == nullptr ) {
        FS_PRINT_WARN( "failed to open file '%s'", fullpath );
    }
    return file;
}

ComFile Com_FsOpenWrite( const char* filename )
{
    ASSERT( filename );

    ComFile file;
    file.handle = std::fopen( filename, "w" );

    if ( file.handle == nullptr ) {
        FS_PRINT_WARN( "failed to open file '%s'", filename );
    }
    return file;
}

void ComFile::Close()
{
    if ( handle ) {
        std::fclose( reinterpret_cast<FILE*>( handle ) );
    }
}

size_t ComFile::Size()
{
    ASSERT( handle );
    FILE* fp = reinterpret_cast<FILE*>( handle );
    fseek( fp, 0L, SEEK_END );
    size_t ret = ftell( fp );
    fseek( fp, 0L, SEEK_SET );
    return ret;
}

ComFile::Result ComFile::Read( char* buffer, size_t size )
{
    ASSERT( handle );
    FILE* file = reinterpret_cast<FILE*>( handle );
    std::fread( buffer, 1, size, file );
    return Result::Ok;
}

ComFile::Result ComFile::Write( char* buffer, size_t size )
{
    ASSERT( handle );
    FILE* file = reinterpret_cast<FILE*>( handle );
    std::fwrite( buffer, size, 1, file );
    return Result::Ok;
}

char* Com_FsAbsolutePath( char* path )
{
    ASSERT( 0 && "TODO: implement" );
    FsReplaceSeparator( path );
    return path;
}
