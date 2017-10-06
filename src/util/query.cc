#include "query.h"

#include <fcntl.h>

#include <cstdio>
#include <cstring>

namespace dictzip {

std::vector<std::string> get_lines_starting_with(
	const std::string& path, const std::string& substring) {
	FILE* file = std::fopen(path.c_str(), "r");

	if ( !file ) {
		return std::vector<std::string>{};
	}

	posix_fadvise(fileno(file), 0, 0, 1);  // FDADVICE_SEQUENTIAL

	std::vector<std::string> result;
	char buffer[16*1024 + 1];
	char* start_of_match = nullptr;

	while ( std::size_t n = std::fread(buffer,
	                                   sizeof(char),
	                                   sizeof(buffer) - 1,
	                                   file) ) {
		for ( char* p = buffer;
			  (p = static_cast<char*>(std::memchr(p, '\n', (buffer + n) - p)));
			  ++p ) {
			if ( start_of_match != nullptr ) {
				result.emplace_back(start_of_match, p - start_of_match);
				start_of_match = nullptr;
			}

			if ( std::strncmp(substring.c_str(), p+1, substring.size()) == 0 ) {
				start_of_match = p+1;
			}
		}
	}

	std::fclose(file);

	return result;
}

std::string get_encoded_offset(const std::string& line) {
	const std::size_t start = line.find_first_of('\t');
	const std::size_t end   = line.find_last_of('\t');

	return line.substr(start + 1, end - (start + 1));
}

std::string get_encoded_length(const std::string& line) {
	const std::size_t start = line.find_last_of('\t');

	return line.substr(start + 1);
}

}
