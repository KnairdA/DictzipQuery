#include "istream/stream.h"
#include "util/base64.h"
#include "util/query.h"

#include <string>
#include <iostream>

std::string get(const std::string& path, std::size_t offset, std::size_t length) {
	dictzip::Istream stream(path.c_str());

	std::string result;
	result.resize(length);

	stream.seekg(offset);
	stream.read(result.data(), length);

	return result;
}

int main(int argc, char** argv) {
	if ( argc != 2 ) {
		std::cerr << "Empty query." << std::endl;
	} else {
		// Get index entries of requested word definitions
		for ( auto& line : dictzip::get_lines_starting_with("gcide.index", argv[1]) ) {
			// Decode location in compressed archive
			const std::size_t offset = dictzip::base64_decode(dictzip::get_encoded_offset(line));
			const std::size_t length = dictzip::base64_decode(dictzip::get_encoded_length(line));

			// Print the GCIDE definition of _Accession_
			std::cout << get("gcide.dict.dz", offset, length) << std::endl;
		}
	}
}
