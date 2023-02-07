#pragma once
#include <string>
#include <vector>
#include "Interface/IRuntimeModule.hpp"

class IAssetLoader : public IRuntimeModule {
   public:
    IAssetLoader() = default;
    virtual ~IAssetLoader() = default;

    virtual std::string BuildSysPath( const char* filename, const char* extraPath ) = 0;

    std::string BuildSysPath( const char* filename )
    {
        return BuildSysPath( filename, nullptr );
    };

    virtual bool AddSearchPath( const std::string& path ) = 0;

    virtual bool RemoveSearchPath( const std::string& path ) = 0;

    virtual void ClearSearchPath() = 0;

    virtual std::vector<char> SyncOpenAndReadText( const char* fileName ) = 0;

    std::vector<char> SyncOpenAndReadText( const std::string& fileName )
    {
        return SyncOpenAndReadText( fileName.c_str() );
    }
};