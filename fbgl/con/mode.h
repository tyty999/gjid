// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// mode.h
//

#ifndef MODE_H_7595D7AD7CC50FC652C46AAA7A9A6F30
#define MODE_H_7595D7AD7CC50FC652C46AAA7A9A6F30

#include "../mode.h"

#define MODEDB_FILE	"/etc/fb.modes"

struct fb_var_screeninfo;

namespace fbgl {

/// \class CConsoleMode mode.h "con/mode.h"
///
/// \brief Console framebuffer mode extension.
///
class CConsoleMode : public CMode {
public:
    inline		CConsoleMode (void)		: CMode() {}
    inline		CConsoleMode (const CMode& v)	: CMode (v) {}
    inline const CMode&	operator= (const CMode& v)	{ CMode::operator= (v); return (*this); }
    void		CreateVarinfo (struct fb_var_screeninfo& vi) const;
    mdbiter_t		ReadFromModedb (const string& i);
};

} // namespace fbgl

#endif

