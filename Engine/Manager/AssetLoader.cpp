#include "AssetLoader.hpp"

#include <filesystem>
#include <fstream>

#include "Base/Asserts.h"
#include "Base/Logger.h"
#include "Core/com_dvars.h"

namespace fs = std::filesystem;
using std::string;
using std::vector;

bool AssetLoader::Initialize()
{
    m_base = Dvar_GetString( fs_base );
    LOG_DEBUG( "AssetLoader base path is '%s'", m_base.c_str() );
    return true;
}

void AssetLoader::Finalize()
{
}

bool AssetLoader::AddSearchPath( const string& path )
{
    if ( m_searchPaths.find( path ) != m_searchPaths.end() ) {
        LOG_WARN( "AssetLoader::AddSearchPath path '%s' already exists", path.c_str() );
        return false;
    }
    m_searchPaths.insert( path );
    return true;
}

bool AssetLoader::RemoveSearchPath( const string& path )
{
    if ( m_searchPaths.find( path ) == m_searchPaths.end() ) {
        LOG_WARN( "AssetLoader::RemoveSearchPath: path '%s' does not exist", path.c_str() );
        return false;
    }
    m_searchPaths.erase( path );
    return true;
}

void AssetLoader::ClearSearchPath()
{
    m_searchPaths.clear();
}

static void ReadTextFileToString(const string& syspath, vector<char>& out) {
    std::ifstream ifs( syspath );
    if ( !ifs.is_open() ) {
        return;
    }
    ifs.seekg( 0, std::ios::end );
    const size_t size = ifs.tellg();
    ifs.seekg( 0 );
    out.resize( size );
    ifs.read( out.data(), size );
}

std::vector<char> AssetLoader::SyncOpenAndReadText( const char* fileName )
{
    fs::path possiblePath = fs::path( m_base ) / fileName;
    vector<char> buffer;
    if ( fs::exists( possiblePath ) ) {
        ReadTextFileToString( possiblePath.string(), buffer );
    }
    else {
        for ( const string& searchPath : m_searchPaths ) {
            possiblePath = fs::path( m_base ) / searchPath / fileName;
            if ( fs::exists( possiblePath ) ) {
                ReadTextFileToString( possiblePath.string(), buffer );
                break;
            }
        }
    }

    if ( buffer.empty() ) {
        LOG_FATAL( "AssetLoader::SyncOpenAndReadTextFileToString: Failed to open text file '%s'", fileName );
    }
    return buffer;
}

std::string AssetLoader::BuildSysPath( const char* filename, const char* extraPath )
{
    fs::path absPath = fs::path( m_base ) / string( extraPath ? extraPath : "" ) / filename;
    return absPath.string();
}
