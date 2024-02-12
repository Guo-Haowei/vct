#pragma once

namespace vct {

std::string open_file_dialog(const std::vector<const char*>& filters);

std::string open_save_dialog(const std::string& file);

}  // namespace vct
