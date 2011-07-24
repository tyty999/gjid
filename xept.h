// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "config.h"
#include <X11/Xlib.h>

class XlibError : public runtime_error {
public:
			XlibError (void) throw();
			XlibError (const XErrorEvent& e) throw();
    inline virtual     ~XlibError (void) throw() {}
    virtual const char*	what (void) const throw();
    virtual void	info (string& msgbuf, const char* fmt = NULL) const throw();
    virtual void	read (istream& is);
    virtual void	write (ostream& os) const;
    virtual size_t	stream_size (void) const;
public:
    XErrorEvent		m_Event;
};
