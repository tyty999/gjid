// Copyright (c) 2003-2006 by Mike Sharov <msharov@users.sourceforge.net>
//
// file.h
//

#ifndef FILE_H_28373C8646D8A2612E21C58D15263115
#define FILE_H_28373C8646D8A2612E21C58D15263115

#include <ustl.h>

struct stat;

namespace fbgl {
using namespace ustl;

/// \class CFile file.h "file.h"
/// \brief File descriptor encapsulation.
///
/// A nonblocking C++ adapter to the libc file interface. Exists
/// primarily to convert all those errno codes into exceptions.
///
class CFile {
public:
    typedef int		fd_t;
    typedef const string& rcname_t;
    enum EMode {
	mode_Read,
	mode_Write,
	mode_Append,
	mode_Modify,
	mode_Last
    };
    static const mode_t	perms_Std = 0644;
    static const fd_t	fd_Null = -1;
public:
			CFile (void);
		       ~CFile (void);
    void		Open (rcname_t name, EMode mode = mode_Read, mode_t perms = perms_Std);
    void		Close (void);
    void		Attach (fd_t fd);
    void		Detach (void);
    size_t		Read (ostream& os);
    size_t		Write (istream& is);
    void		Stat (struct stat& st) const;
    void		Seek (off_t p);
    void		Sync (void);
    void		Ioctl (const char* name, int request, long argument);
    memlink		Map (size_t size, off_t offset = 0);
    void		Unmap (memlink& l);
    void		MSync (memlink& l);
    inline void		Ioctl (const char* name, int request, void* argument)	{ Ioctl (name, request, intptr_t(argument)); }
    inline fd_t		Fd (void) const		{ return (m_fd); }
    inline rcname_t	Name (void) const	{ return (m_Name); }
    inline bool		IsOpen (void) const	{ return (m_fd != fd_Null); }
private:
    inline		CFile (const CFile&) {}
    inline void		operator= (const CFile&) const {}
private:
    string		m_Name;		///< File name, empty if not owned.
    fd_t		m_fd;		///< Active file descriptor.
    int			m_Flags;	///< Active file flags.
    bool		m_bOwned;	///< If the object owns the file descriptor.
};

#define IOCTLID(bit)	"ioctl("#bit")", bit

} // namespace fbgl

#endif

