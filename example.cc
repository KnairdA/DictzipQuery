#include "istream/stream.h"
#include "util/base64.h"

#include <string>
#include <iostream>

int main() {
	dictzip::Istream stream("gcide.dict.dz");

	// Decode location of _Accession_
	const std::size_t offset = dictzip::base64_decode("8Aw");
	const std::size_t length = dictzip::base64_decode("Wt");

	// Print the GCIDE definition of _Accession_
	std::string data;
	data.reserve(length);

	stream.seekg(offset);
	stream.read(const_cast<char*>(data.data()), length);

	std::cout << data.c_str() << std::endl;
}
