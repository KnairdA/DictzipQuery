#include <iostream>

#include "stream.h"

namespace dictzip {

Istream::Istream(char const* filename):
	std::istream(0),
	buffer_{ new IstreamBuf(filename) } {
	this->rdbuf(buffer_.get());
}

}
