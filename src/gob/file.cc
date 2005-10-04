// Copyright (c) 2003-2006 by Mike Sharov <msharov@users.sourceforge.net>
//
// file.cc
//

#include "file.h"
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

namespace fbgl {

//----------------------------------------------------------------------

const mode_t CFile::perms_Std;
const CFile::fd_t CFile::fd_Null;

//----------------------------------------------------------------------

/// Default constructor.
CFile::CFile (void)
: m_Name (),
  m_fd (fd_Null),
  m_Flags (0),
  m_bOwned (false)
{
}

/// Default destructor.
CFile::~CFile (void)
{
    if (m_bOwned)
	Close();
}

/// Opens file \p name with \p mode and \p perms.
void CFile::Open (rcname_t name, EMode mode, mode_t perms)
{
    static const int openmode [mode_Last] = {
	/* mode_Read */		O_RDONLY,
	/* mode_Write */	O_WRONLY | O_CREAT | O_TRUNC,
	/* mode_Append */	O_WRONLY | O_CREAT | O_APPEND,
	/* mode_Modify */	O_RDWR
    };
    Close();
    const int fd = open (name, openmode[mode], perms);
    if (fd < 0)
	throw file_exception ("open", name.c_str());
    m_bOwned = true;
    m_fd = fd;
    m_Flags = mode;
    m_Name = name;
}

/// Closes the file descriptor.
void CFile::Close (void)
{
    if (m_bOwned && close (m_fd) < 0)
	throw file_exception ("close", m_Name.c_str());
    m_bOwned = false;
    m_fd = fd_Null;
    m_Flags = 0;
    m_Name.clear();
}

/// Attaches to \p fd.
void CFile::Attach (int fd)
{
    int flags;
    if ((flags = fcntl (fd, F_GETFL)) < 0)
	throw file_exception ("fcntl(F_GETFL)", m_Name.c_str());
    if (!(flags & O_NONBLOCK) && fcntl (fd, F_SETFL, flags |= O_NONBLOCK) < 0)
	throw file_exception ("fcntl(F_SETFL)", m_Name.c_str());
    Close();
    m_fd = fd;
    m_Flags = flags;
    static const char* c_FdNames[] = { "stdin", "stdout", "stderr", "fd[%d]" };
    m_Name.format (c_FdNames [min (size_t(m_fd), VectorSize(c_FdNames)-1)], m_fd);
}

/// Detaches from the file descriptor without closing it.
void CFile::Detach (void)
{
    m_bOwned = false;
    Close();
}

/// Reads data into \p os.
size_t CFile::Read (ostream& os)
{
    const size_t wasRemaining (os.remaining());
    while (os.remaining()) {
	ssize_t br = read (m_fd, os.ipos(), os.remaining());
	if (br == 0)
	    throw file_exception ("read eof", m_Name.c_str());
	else if (br > 0)
	    os.skip (br);
	else {
	    if (errno == EAGAIN)
		break;
	    else if (errno != EINTR)
		throw file_exception ("read", m_Name.c_str());
	}
    }
    return (wasRemaining - os.remaining());
}

/// Writes data from \p os.
size_t CFile::Write (istream& is)
{
    const size_t wasRemaining (is.remaining());
    while (is.remaining()) {
	ssize_t bw = write (m_fd, is.ipos(), is.remaining());
	if (bw > 0)
	    is.skip (bw);
	else { // This also throws when write returns 0, which is always a bad thing.
	    if (errno == EAGAIN)
		break;
	    else if (errno != EINTR)
		throw file_exception ("write", m_Name.c_str());
	}
    }
    return (wasRemaining - is.remaining());
}

void CFile::Stat (struct stat& st) const
{
    if (fstat (m_fd, &st) < 0)
	throw file_exception ("fstat", m_Name.c_str());
}

/// Seeks to the requested offset.
void CFile::Seek (off_t p)
{
    if (lseek (m_fd, p, SEEK_SET) < 0)
	throw file_exception ("lseek", m_Name.c_str());
}

/// Flushes the data to disk.
void CFile::Sync (void)
{
    if (fsync (m_fd) < 0)
	throw file_exception ("fsync", m_Name.c_str());
}

/// Executes the requested ioctl.
void CFile::Ioctl (const char* name, int request, long argument)
{
    if (ioctl (m_fd, request, argument) < 0)
	throw file_exception (name, m_Name.c_str());
}

/// Maps the file to a memory location.
memlink CFile::Map (size_t size, off_t offset)
{
    void* result = mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, offset);
    if (result == MAP_FAILED)
	throw file_exception ("mmap", m_Name.c_str());
    return (memlink (result, size));
}

/// Unmaps the given memory location.
void CFile::Unmap (memlink& l)
{
    if (munmap (l.data(), l.size()) < 0)
	throw file_exception ("munmap", m_Name.c_str());
    l.unlink();
}

/// Synchronizes a mapped location.
void CFile::MSync (memlink& l)
{
    if (msync (l.data(), l.size(), MS_ASYNC | MS_INVALIDATE))
	throw file_exception ("msync", m_Name.c_str());
}

} // namespace fbgl

