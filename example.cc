#include "istream/stream.h"

#include <string>
#include <iostream>

int main() {
	dictzip::Istream stream("gcide.dict.dz");

	// Print the GCIDE definition of _Accession_
	std::string data;
	data.reserve(1453);

	stream.seekg(245808);
	stream.read(const_cast<char*>(data.data()), 1453);

	std::cout << data.c_str() << std::endl;
}
