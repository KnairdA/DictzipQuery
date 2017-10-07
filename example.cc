#include "index.h"
#include "istream/stream.h"

#include <string>
#include <iostream>

std::string get(const std::string& path, std::size_t offset, std::size_t length) {
	dictzip::Istream stream(path.c_str());

	std::string result;
	result.resize(length);

	stream.seekg(offset);
	stream.read(&result[0], length);

	return result;
}

int main(int argc, char** argv) {
	if ( argc != 2 ) {
		std::cerr << "Empty query." << std::endl;
	} else {
		dictzip::IndexFile index("gcide.index");

		// Get index entries of requested word definitions
		for ( auto& entry : index.get(argv[1]) ) {
			// Print the GCIDE definition
			std::cout << get("gcide.dict.dz", entry.offset, entry.length) << std::endl;
		}
	}
}
