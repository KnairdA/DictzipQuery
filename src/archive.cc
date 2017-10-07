#include "archive.h"

#include "istream/stream.h"

namespace dictzip {

ArchiveFile::ArchiveFile(const std::string& path):
   path_(path) { }

std::string ArchiveFile::get(std::size_t offset, std::size_t length) const {
	Istream stream(this->path_.c_str());

	std::string result;
	result.resize(length);

	stream.seekg(offset);
	stream.read(&result[0], length);

	return result;
}

std::string ArchiveFile::get(const IndexFile::Entry& entry) const {
	return this->get(entry.offset, entry.length);
}

}
