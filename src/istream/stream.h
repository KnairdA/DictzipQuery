#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "buffer.h"

namespace dictzip {

class Istream : public std::istream {
private:
	std::shared_ptr<IstreamBuf> buffer_;

public:
	Istream(char const* filename);
	virtual ~Istream() {}

};

}
