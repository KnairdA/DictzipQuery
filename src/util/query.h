#pragma once

#include <string>

namespace dictzip {

std::string get_line_starting_with(const std::string& path, const std::string& substring);

std::string get_encoded_offset(const std::string& line);
std::string get_encoded_length(const std::string& line);

}
