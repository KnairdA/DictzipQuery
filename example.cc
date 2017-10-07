#include "index.h"
#include "archive.h"

#include <string>
#include <iostream>

int main(int argc, char** argv) {
	if ( argc != 2 ) {
		std::cerr << "Empty query." << std::endl;
	} else {
		dictzip::IndexFile index("gcide.index");
		dictzip::ArchiveFile archive("gcide.dict.dz");

		for ( auto& entry : index.get(argv[1]) ) {
			std::cout << archive.get(entry) << std::endl;
		}
	}
}
