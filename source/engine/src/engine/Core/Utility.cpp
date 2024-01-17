#include "Utility.h"
#include <fstream>

#include "Log.h"

std::string read_file_to_buffer(const char* path)
{
    if (!fs::exists(fs::path(path)))
    {
        LOG_ERROR("File {} does not exist", path);
        return "";
    }

    std::string result;
    std::ifstream file(path, std::ios::binary);
    if (file.eof() || file.fail())
    {
        return result;
    }

    file.seekg(0, std::ios_base::end);
    size_t fileSize = file.tellg();
    result.resize(fileSize);

    file.seekg(0, std::ios_base::beg);
    file.read(result.data(), fileSize);
    return result;
}
