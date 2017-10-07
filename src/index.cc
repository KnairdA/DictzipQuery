#include "index.h"

#include "util/query.h"
#include "util/base64.h"

#include <algorithm>


namespace dictzip {

IndexFile::Entry parse_from_line(const std::string& line) {
	const std::size_t start = line.find_first_of('\t');
	const std::size_t end   = line.find_last_of('\t');

	return IndexFile::Entry(
		line.substr(0, start),
		base64_decode(line.substr(start + 1, end - (start + 1))),
		base64_decode(line.substr(end + 1)));
}

IndexFile::Entry::Entry(const std::string& line):
	IndexFile::Entry{parse_from_line(line)} { }

IndexFile::Entry::Entry(
	const std::string& word, std::size_t offset, std::size_t length):
	word(word),
	offset(offset),
	length(length) { }

IndexFile::IndexFile(const std::string& path):
	path_(path) { }

std::vector<IndexFile::Entry> IndexFile::get(const std::string& word) {
	const std::vector<std::string> lines = get_lines_starting_with(this->path_, word);

	std::vector<Entry> entries;
	entries.reserve(lines.size());

	std::for_each(
		lines.begin(),
		lines.end(),
		[&entries](const std::string& line) {
			entries.emplace_back(line);
		}
	);

	return entries;
}

}
