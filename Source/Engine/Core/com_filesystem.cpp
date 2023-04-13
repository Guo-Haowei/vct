#include "com_filesystem.h"

#include <cstring>
#include <filesystem>

#include "Core/Check.h"
#include "Core/Log.h"

#define PRINT_FILESYSTEM IN_USE
#if USING(PRINT_FILESYSTEM)
#define FS_PRINT(fmt, ...)         LOG_LEVEL(ELogLevel::Info, "[filesystem] " fmt, ##__VA_ARGS__)
#define FS_PRINT_WARNING(fmt, ...) LOG_LEVEL(ELogLevel::Warn, "[filesystem] " fmt, ##__VA_ARGS__)
#else
#define FS_PRINT(fmt, ...)         ((void)0)
#define FS_PRINT_WARNING(fmt, ...) ((void)0)
#endif

#ifndef ROOT_FOLDER
#define ROOT_FOLDER ""
#endif

static struct
{
    char base[kMaxOSPath];
    bool initialized;
} s_glob;

static void FsReplaceSeparator(char* path)
{
    for (char* it = path; *it; ++it)
    {
        if (*it == '\\')
        {
            *it = '/';
        }
    }
}

bool Com_FsInit()
{
    if (s_glob.initialized)
    {
        return true;
    }

    // setup basepath
    strncpy(s_glob.base, ROOT_FOLDER, sizeof(s_glob.base));
    FsReplaceSeparator(s_glob.base);

    FS_PRINT("base path is '{}'", s_glob.base);

    s_glob.initialized = true;
    return true;
}

void Com_FsBuildPath(char* buf, size_t bufLen, const char* filename, const char* path)
{
    check(s_glob.initialized);
    snprintf(buf, bufLen, "%s%s/%s", s_glob.base, path ? path : "", filename);
}

ComFile Com_FsOpenRead(const char* filename, const char* path)
{
    check(s_glob.initialized);
    check(filename);

    char fullpath[kMaxOSPath];
    // try relative
    snprintf(fullpath, kMaxOSPath, "%s/%s", path ? path : ".", filename);
    if (!std::filesystem::exists(fullpath))
    {
        Com_FsBuildPath(fullpath, kMaxOSPath, filename, path);
    }

    ComFile file;
    file.handle = std::fopen(fullpath, "r");
    if (file.handle == nullptr)
    {
        FS_PRINT_WARNING("failed to open file '%s'", fullpath);
    }
    return file;
}

ComFile Com_FsOpenWrite(const char* filename)
{
    check(filename);

    ComFile file;
    file.handle = std::fopen(filename, "w");

    if (file.handle == nullptr)
    {
        FS_PRINT_WARNING("failed to open file '%s'", filename);
    }
    return file;
}

void ComFile::Close()
{
    if (handle)
    {
        std::fclose(reinterpret_cast<FILE*>(handle));
    }
}

size_t ComFile::Size()
{
    check(handle);
    FILE* fp = reinterpret_cast<FILE*>(handle);
    fseek(fp, 0L, SEEK_END);
    size_t ret = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return ret;
}

ComFile::Result ComFile::Read(char* buffer, size_t size)
{
    check(handle);
    FILE* file = reinterpret_cast<FILE*>(handle);
    std::fread(buffer, 1, size, file);
    return Result::Ok;
}

ComFile::Result ComFile::Write(char* buffer, size_t size)
{
    check(handle);
    FILE* file = reinterpret_cast<FILE*>(handle);
    std::fwrite(buffer, size, 1, file);
    return Result::Ok;
}

char* Com_FsAbsolutePath(char* path)
{
    check(0 && "TODO: implement");
    FsReplaceSeparator(path);
    return path;
}
