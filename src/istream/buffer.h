#pragma once

#include <cstdio>
#include <iostream>
#include <streambuf>
#include <vector>

namespace dictzip {

// Warning: The inherent statefulness of stream buffers interferes with
// multithreaded access. Users of this class should perform appropriate
// locking, since they are in a better position to do so.
class IstreamBuf : public std::streambuf {
private:
	struct Chunk {
		Chunk(size_t offset, size_t size):
			offset(offset),
			size(size) {};

		const size_t offset;
		const size_t size;
	};

	FILE* dictzip_file_;

	std::vector<unsigned char> buffer_;
	std::vector<unsigned char> header_;
	std::vector<Chunk>  chunks_;

	size_t chunk_length_;
	long data_offset_;
	long curr_chunk_;

	void readChunk(long n);
	void readHeader();
	void readExtra();
	void skipOptional();

protected:
	int underflow();
	std::streamsize xsgetn(char *dest, std::streamsize n);

public:
	using pos_type = std::streambuf::pos_type;
	using off_type = std::streambuf::off_type;

	typedef std::ios::seekdir  seekdir;
	typedef std::ios::openmode openmode;

	IstreamBuf(char const* filename);
	~IstreamBuf();

	pos_type seekoff(off_type off, seekdir dir, openmode);
	pos_type seekpos(pos_type off, openmode mode);

};

}
