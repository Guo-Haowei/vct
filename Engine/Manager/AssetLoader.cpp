#include "AssetLoader.hpp"

#include <filesystem>

#include "Base/Asserts.h"
#include "Base/Logger.h"

#include "Core/com_dvars.h"

#define PRINT_FILESYSTEM IN_USE
#if USING( PRINT_FILESYSTEM )
#define FS_PRINT_WARN( fmt, ... ) ::base::Log( ::base::Level::Warn, "[filesystem] " fmt, ##__VA_ARGS__ )
#else
#define FS_PRINT_WARN( fmt, ... ) ( (void)0 )
#endif

namespace fs = std::filesystem;
using std::string;

FileManager* g_fileMgr = new FileManager();

void SystemFile::Close()
{
    if ( handle ) {
        std::fclose( reinterpret_cast<FILE*>( handle ) );
    }
}

size_t SystemFile::Size()
{
    ASSERT( handle );
    FILE* fp = reinterpret_cast<FILE*>( handle );
    fseek( fp, 0L, SEEK_END );
    size_t ret = ftell( fp );
    fseek( fp, 0L, SEEK_SET );
    return ret;
}

SystemFile::Result SystemFile::Read( char* buffer, size_t size )
{
    ASSERT( handle );
    FILE* file = reinterpret_cast<FILE*>( handle );
    std::fread( buffer, 1, size, file );
    return Result::Ok;
}

SystemFile::Result SystemFile::Write( char* buffer, size_t size )
{
    ASSERT( handle );
    FILE* file = reinterpret_cast<FILE*>( handle );
    std::fwrite( buffer, size, 1, file );
    return Result::Ok;
}
bool FileManager::Initialize()
{
    m_base = Dvar_GetString( fs_base );
    LOG_DEBUG( "base path is '%s'", m_base.c_str() );
    return true;
}

void FileManager::Finalize()
{
}

SystemFile FileManager::OpenRead( const char* filename, const char* path )
{
    ASSERT( filename );

    // try relative
    std::string fullpath;
    char tmp[1024];
    snprintf( tmp, sizeof( fullpath ), "%s/%s", path ? path : ".", filename );

    if ( std::filesystem::exists( fullpath ) ) {
        fullpath = tmp;
    }
    else {
        fullpath = BuildAbsPath( filename, path );
    }

    SystemFile file;
    file.handle = std::fopen( fullpath.c_str(), "r" );
    if ( file.handle == nullptr ) {
        FS_PRINT_WARN( "failed to open file '%s'", fullpath.c_str() );
    }
    return file;
}

SystemFile FileManager::OpenWrite( const char* filename )
{
    ASSERT( filename );

    SystemFile file;
    file.handle = std::fopen( filename, "w" );

    if ( file.handle == nullptr ) {
        FS_PRINT_WARN( "failed to open file '%s'", filename );
    }
    return file;
}

std::string FileManager::BuildAbsPath( const char* filename, const char* extraPath )
{
    fs::path absPath = fs::path( m_base ) / string( extraPath ? extraPath : "" ) / filename;
    return absPath.string();
}
