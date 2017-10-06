#include "base64.h"

#include <vector>
#include <stdexcept>

namespace dictzip {

std::size_t base64_decode(const std::string& encoded) {
	std::vector<std::int8_t> map(256,-1);

	for ( int i = 0; i < 64; ++i ) {
		map["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
	}

	std::size_t value = 0;

	for ( std::uint8_t c : encoded ) {
		if ( map[c] == -1 ) {
			throw std::invalid_argument("Invalid character in BASE64 string.");
		} else {
			value *= 64;
			value += map[c];
		}
	}

	return value;
}

}
