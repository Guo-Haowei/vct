#pragma once

namespace vct {

std::string open_file_dialog(const std::vector<const char*>& filters);

bool open_save_dialog(std::filesystem::path& inout_path);

}  // namespace vct
