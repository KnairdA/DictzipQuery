#pragma once

#include <string>
#include <vector>

namespace dictzip {

std::vector<std::string> get_lines_starting_with(
	const std::string& path, const std::string& substring);

std::string get_encoded_offset(const std::string& line);
std::string get_encoded_length(const std::string& line);

}
