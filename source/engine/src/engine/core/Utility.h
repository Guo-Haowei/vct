#pragma once

namespace fs = std::filesystem;

std::string read_file_to_buffer(const char* path);

inline std::string read_file_to_buffer(const std::string& path) { return read_file_to_buffer(path.c_str()); }

inline std::string read_file_to_buffer(const fs::path& path) { return read_file_to_buffer(path.string()); }
