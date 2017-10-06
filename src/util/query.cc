#include "query.h"

#include <fcntl.h>

#include <cstdio>
#include <cstring>

#include <stdexcept>

namespace dictzip {

std::string get_line_starting_with(
	const std::string& path, const std::string& substring) {
	static const auto BUFFER_SIZE = 16*1024;

	FILE* file = std::fopen(path.c_str(), "r");

	posix_fadvise(fileno(file), 0, 0, 1);  // FDADVICE_SEQUENTIAL

	char buffer[BUFFER_SIZE + 1];
	char* start_of_match = nullptr;

	while( std::size_t bytes_read = std::fread(buffer, sizeof(char), BUFFER_SIZE, file) ) {
		if ( bytes_read ) {
			for ( char* p = buffer;
			      (p = static_cast<char*>(std::memchr(p, '\n', (buffer + bytes_read) - p)));
			      ++p ) {
				if ( start_of_match == nullptr ) {
					if ( std::strncmp(substring.c_str(), p+1, substring.size()) == 0 ) {
						start_of_match = p+1;
					}
				} else {
					return std::string(start_of_match, p-start_of_match);
				}
			}
		} else {
			break;
		}
	}

	throw std::runtime_error("No match found");
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
