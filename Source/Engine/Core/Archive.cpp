#include "Archive.h"

#include "Core/Check.h"

bool Archive::OpenRead(const std::string& path)
{
    mFile = fopen(path.c_str(), "rb");

    if (mFile == NULL)
    {
        return false;
    }

    mMode = EMode::Read;
    return true;
}

bool Archive::OpenWrite(const std::string& path)
{
    mFile = fopen(path.c_str(), "wb");  // r for read, b for binary

    if (mFile == NULL)
    {
        return false;
    }

    mMode = EMode::Write;
    return true;
}

void Archive::Close()
{
    switch (mMode)
    {
        case EMode::Write:
            if (mFile)
            {
                fclose(mFile);
                mFile = nullptr;
            }
            break;
        case EMode::Read:
            if (mFile)
            {
                fclose(mFile);
                mFile = nullptr;
            }
            break;
        default:
            break;
    }

    mMode = EMode::None;
}

bool Archive::IsWriteMode() const
{
    check(mFile && mMode != EMode::None);
    return mMode == EMode::Write;
}

bool Archive::Write(const void* data, size_t bytes)
{
    check(mMode == EMode::Write && mFile);
    size_t written = fwrite(data, 1, bytes, mFile);
    if (written == bytes)
    {
        return true;
    }

    LOG_FATAL("Attemp to write {} bytes, actually {} bytes written", bytes, written);
    return false;
}

bool Archive::Read(void* data, size_t bytes)
{
    check(mMode == EMode::Read && mFile);
    size_t read = fread(data, 1, bytes, mFile);
    if (read == bytes)
    {
        return true;
    }

    LOG_FATAL("Attemp to read {} bytes, actually {} bytes read", bytes, read);
    return false;
}