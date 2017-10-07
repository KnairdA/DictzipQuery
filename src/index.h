#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace dictzip {

class IndexFile {
public:
	struct Entry {
		Entry(const std::string& line);
		Entry(const std::string& word, std::size_t offset, std::size_t length);

		const std::string word;
		const std::size_t offset;
		const std::size_t length;
	};

	IndexFile(const std::string& path);

	std::vector<Entry> get(const std::string& word);

private:
	const std::string path_;

};

}
