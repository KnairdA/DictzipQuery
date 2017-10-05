#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include <zlib.h>

#include "buffer.h"
#include "util/gzip.h"

namespace dictzip {

void IstreamBuf::readChunk(long n) {
	if ( n == curr_chunk_ ) {
		return;
	}

	const IstreamBuf::Chunk chunkN = chunks_[n];
	unsigned char* zBuf = new unsigned char[chunkN.size];
	
	std::fseek(dictzip_file_, data_offset_ + chunkN.offset, SEEK_SET);

	if ( fread(zBuf, 1, chunkN.size, dictzip_file_) != chunkN.size ) {
		throw std::runtime_error("Could not read dictzip chunk.");
	}

	z_stream zStream;
	zStream.next_in   = zBuf;
	zStream.avail_in  = chunkN.size;
	zStream.next_out  = &buffer_[0];
	zStream.avail_out = chunk_length_;
	zStream.zalloc    = NULL;
	zStream.zfree     = NULL;

	if ( inflateInit2(&zStream, -15) != Z_OK ) {
		delete[] zBuf;
		throw std::runtime_error(zStream.msg);
	}

	const int r = inflate(&zStream, Z_PARTIAL_FLUSH);

	if ( r != Z_OK && r != Z_STREAM_END ) {
		delete[] zBuf;
		throw std::runtime_error(zStream.msg);
	}
	
	delete[] zBuf;

	if ( inflateEnd(&zStream) != Z_OK ) {
		throw std::runtime_error(zStream.msg);
	}

	unsigned char *buffer = &buffer_[0];

	this->setg(
		reinterpret_cast<char*>(buffer),
		reinterpret_cast<char*>(buffer),
		reinterpret_cast<char*>(buffer) + zStream.total_out
	);
	
	curr_chunk_ = n;
}

void IstreamBuf::readExtra() {
	const int  extraLen  = std::fgetc(dictzip_file_)
	                     + ( std::fgetc(dictzip_file_) * 256 );
	const long extraPos  = std::ftell(dictzip_file_);
	const long nextField = extraPos + extraLen;

	while ( std::ftell(dictzip_file_) < nextField ) {
		// Read extra field 'header'
		char si[2];
		if ( std::fread(si, 1, sizeof(si), dictzip_file_) != sizeof(si) ) {
			throw std::runtime_error("Could not read extra dictzip field header.");
		}

		const int len = std::fgetc(dictzip_file_)
		              + ( std::fgetc(dictzip_file_) * 256 );
		
		// Check for chunk information
		if ( si[0] == 'R' && si[1] == 'A' ) {
			const int ver = std::fgetc(dictzip_file_)
			              + ( std::fgetc(dictzip_file_) * 256 );
			if ( ver != 1 ) {
				throw std::runtime_error("Unknown dictzip version.");
			}
			
			chunk_length_ = std::fgetc(dictzip_file_)
			              + ( std::fgetc(dictzip_file_) * 256 );
			const size_t chunkCount = std::fgetc(dictzip_file_)
			                        + ( std::fgetc(dictzip_file_) * 256 );
			
			buffer_.resize(chunk_length_);
			unsigned char *buffer = &buffer_[0];

			this->setg(
				reinterpret_cast<char*>(buffer),
				reinterpret_cast<char*>(buffer) + chunk_length_,
				reinterpret_cast<char*>(buffer) + chunk_length_
			);
			
			size_t chunkPos = 0;

			for ( size_t i = 0; i < chunkCount; ++i ) {
				const size_t chunkLen = std::fgetc(dictzip_file_)
				                      + ( std::fgetc(dictzip_file_) * 256 );
				chunks_.emplace_back(chunkPos, chunkLen);	
				chunkPos += chunkLen;
			}
		} else {
			std::fseek(dictzip_file_, len, SEEK_CUR);
		}
	}
}

void IstreamBuf::readHeader() {
	header_.resize(GZ_HEADER_SIZE);
	unsigned char* header = &header_[0];

	if ( std::fread(header, 1, GZ_HEADER_SIZE, dictzip_file_) != GZ_HEADER_SIZE ) {
		throw std::runtime_error("Could not read dictzip header.");
	}
	
	if ( header[GZ_HEADER_ID1] != gzipId1 ||
	     header[GZ_HEADER_ID2] != gzipId2 ) {
		throw std::runtime_error("Given dictzip file is not a gzip file.");
	}
	
	if ( header[GZ_HEADER_CM] != GZ_CM_DEFLATE ) {
		throw std::runtime_error("Unknown compression method detected.");
	}
	
	if ( !(header[GZ_HEADER_FLG] & GZ_FLG_EXTRA) ) {
		throw std::runtime_error("No extra fields, given file cannot be a dictzip file.");
	}
}

void IstreamBuf::skipOptional() {
	const unsigned char* header = &header_[0];
	
	if ( header[GZ_HEADER_FLG] & GZ_FLG_NAME ) {
		while ( std::fgetc(dictzip_file_) != 0 ) {}
	}
		
	if ( header[GZ_HEADER_FLG] & GZ_FLG_COMMENT ) {
		while ( std::fgetc(dictzip_file_) != 0 ) {}
	}
	
	if ( header[GZ_HEADER_FLG] & GZ_FLG_HCRC ) {
		std::fseek(dictzip_file_, 2, SEEK_CUR);
	}
}

int IstreamBuf::underflow() {
	if ( this->gptr() < this->egptr() ) {
		return *this->gptr();
	}

	if ( curr_chunk_ + 1 >= static_cast<int>(chunks_.size()) ) {
		return EOF;
	}

	this->readChunk(curr_chunk_ + 1);
	
	return *gptr();
}

// From C++ annotations 8.1.0~pre1, chapter 23.
std::streamsize IstreamBuf::xsgetn(char *dest, std::streamsize n) {
	int nread = 0;
	
	while ( n ) {
		if ( !this->in_avail() ) {
			if ( this->underflow() == EOF ) {
				break;
			}
		}
		
		int avail = this->in_avail();
		
		if (avail > n) {
			avail = n;
		}
		
		std::memcpy(dest + nread, gptr(), avail);
		this->gbump(avail);
		
		nread += avail;
		n     -= avail;
	}
	
	return nread;
}

IstreamBuf::IstreamBuf(char const *filename):
	dictzip_file_{ fopen(filename, "r") } {
	if ( dictzip_file_ ) {
		readHeader();
		readExtra();
		skipOptional();
		data_offset_ = std::ftell(dictzip_file_);
		curr_chunk_ = -1;
	} else {
		throw std::runtime_error("Could not open input dictzip stream.");
	}
}

IstreamBuf::~IstreamBuf() {
	if ( dictzip_file_ ) {
		fclose(dictzip_file_);
	}
}

IstreamBuf::pos_type IstreamBuf::seekoff(off_type off, seekdir dir, openmode) {
	pos_type targetPos;

	switch ( dir ) {
		case std::ios::beg: {
			targetPos = off;
			break;
		}
		case std::ios::cur: {
			const pos_type curPos = (curr_chunk_ * chunk_length_)
			                      + (gptr() - eback());
			targetPos = curPos + off;
			break;
		}
		default: {
			// XXX - We can only detmine the uncompressed file length by decompressing the
			// last chunk. Quite inefficient, haven't made my mind up whether we want to
			// support this.
			return EOF;
		}
	}

	if ( targetPos < 0 ) {
		return -1;
	} else {
		const int targetChunk = targetPos / chunk_length_;
		const int chunkPos    = targetPos % chunk_length_;
	
		this->readChunk(targetChunk);
		this->setg(this->eback(), this->eback() + chunkPos, this->egptr());
		
		return targetPos;
	}
}

IstreamBuf::pos_type IstreamBuf::seekpos(pos_type off, openmode mode) {
	return seekoff(off, std::ios::beg, mode);
}

}
