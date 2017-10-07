#pragma once

#include "index.h"

namespace dictzip {

class ArchiveFile {
public:
	ArchiveFile(const std::string& path);

	std::string get(std::size_t offset, std::size_t length) const;
	std::string get(const IndexFile::Entry& entry) const;

private:
	const std::string path_;

};

}
