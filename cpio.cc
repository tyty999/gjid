// Copyright (c) 2011 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "cpio.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

namespace {

// CPIO file format header copied from the spec
class CpioHeader {
public:
    inline		CpioHeader (void)	{ }
    inline uint32_t	Filesize (void) const	{ return ((uint32_t(filesize[0]) << 16) | filesize[1]); }
    inline bool		MagicOk (void) const	{ return (magic == 070707); }
    inline const char*	Filename (void) const	{ return ((const char*)this + sizeof(*this)); }
    inline istream	Filedata (void) const	{ return (istream (Filename()+Align(namesize,2), Filesize())); }
    inline const CpioHeader* Next (void) const	{ return ((const CpioHeader*)(Filename()+Align(Filesize(),2)+Align(namesize,2))); }
private:
    uint16_t		magic;		///< Magic id 070707
    uint16_t		dev;
    uint16_t		ino;
    uint16_t		mode;
    uint16_t		uid;
    uint16_t		gid;
    uint16_t		nlink;
    uint16_t		rdev;
    uint16_t		mtime[2];
    uint16_t		namesize;	///< Length of name right after the header, includes zero terminator. Name is padded to even offset.
    uint16_t		filesize[2];	///< Length of the file data after the name. Also padded to even offset.
};
} // namespace

/// Opens \p filename as a CPIO file
CPIO::CPIO (const char* filename)
: _pData (NULL)
{
    _fd = open (filename, O_RDONLY);	// Any errors here will result in mmap failure and _pData being NULL
    struct stat st;
    fstat (_fd, &st);
    _pData = (pointer) mmap (NULL, _dataSize = st.st_size, PROT_READ, MAP_PRIVATE, _fd, 0);
    if (_pData == MAP_FAILED)
	_pData = NULL;
}

/// Closes the file and frees all memory
CPIO::~CPIO (void)
{
    munmap (_pData, _dataSize);
    close (_fd);
}

/// Returns the stream with \p filename data. If not found, an empty stream is returned.
istream CPIO::File (const char* filename)
{
    if (!_pData)
	return (istream());
    typedef const CpioHeader* pch_t;
    for (pch_t pi = (pch_t) begin(), pe = (pch_t) end(); pi < pe; pi = pi->Next())
	if (!strcmp (pi->Filename(), filename))
	    return (pi->Filedata());
    return (istream());
}
