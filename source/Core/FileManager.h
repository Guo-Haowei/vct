#pragma once
#include <string>

#include "Base/IManager.h"

struct SystemFile {
    enum class Result {
        Ok
    };

    enum class Mode {
        ReadOnly,
        ReadWrite,
    };

    void* handle;

    void Close();
    size_t Size();
    Result Read( char* buffer, size_t size );
    template<class T>
    Result Read( T& buffer )
    {
        const size_t size = Size();
        buffer.resize( size );
        return Read( buffer.data(), size );
    }

    Result Write( char* buffer, size_t size );
};

class SystemFileWrapper {
    SystemFile file;

public:
    SystemFileWrapper( SystemFile file )
        : file( file )
    {
    }

    SystemFileWrapper( const SystemFileWrapper& ) = delete;

    ~SystemFileWrapper()
    {
        file.Close();
    }

    bool IsGood() const { return file.handle != nullptr; }

    inline size_t Size() { return file.Size(); }
    inline SystemFile::Result Read( char* buffer, size_t size ) { return file.Read( buffer, size ); }
    template<class T>
    inline SystemFile::Result Read( T& buffer )
    {
        return file.Read<T>( buffer );
    }

    inline SystemFile::Result Write( char* buffer, size_t size ) { return file.Write( buffer, size ); }
};

class FileManager : public IManager {
public:
    FileManager()
        : IManager( "FileManager" )
    {
    }

    virtual bool Init() override;
    virtual void Deinit() override;

    SystemFile OpenRead( const char* filename, const char* path = nullptr );
    SystemFile OpenWrite( const char* filename );

    std::string BuildAbsPath( const char* filename, const char* extraPath = nullptr );

private:
    std::string m_base;
};

extern FileManager* g_fileMgr;
