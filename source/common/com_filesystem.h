#pragma once
#include <vector>

inline constexpr size_t kMaxOSPath = 256;

bool Com_FsInit();

struct ComFile {
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

ComFile Com_FsOpenRead( const char* filename, const char* path = nullptr );
ComFile Com_FsOpenWrite( const char* filename );

void Com_FsBuildPath( char* buf, size_t bufLen, const char* filename, const char* path = nullptr );

char* Com_FsAbsolutePath( char* path );

class ComFileWrapper {
    ComFile file;

   public:
    ComFileWrapper( ComFile file )
        : file( file )
    {
    }

    ComFileWrapper( const ComFileWrapper& ) = delete;

    ~ComFileWrapper()
    {
        file.Close();
    }

    bool IsGood() const { return file.handle != nullptr; }

    inline size_t Size() { return file.Size(); }
    inline ComFile::Result Read( char* buffer, size_t size ) { return file.Read( buffer, size ); }
    template<class T>
    inline ComFile::Result Read( T& buffer )
    {
        return file.Read<T>( buffer );
    }

    inline ComFile::Result Write( char* buffer, size_t size ) { return file.Write( buffer, size ); }
};
