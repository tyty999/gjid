// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "../mode.h"
#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>

namespace fbgl {

/// \class CXlibMode mode.h "con/mode.h"
///
/// \brief Xlib framebuffer mode extension.
///
class DLL_LOCAL CXlibMode : public CMode {
public:
    /// Bits for flags in XF86VidModeModeInfo
    enum XModeFlagBits {
	xflag_PosHSync,
	xflag_NegHSync,
	xflag_PosVSync,
	xflag_NegVSync,
	xflag_Interlaced,
	xflag_Doublescan,
	xflag_CSync,
	xflag_PosCSync,
	xflag_NegCSync,
	xflag_HSkew,
	xflag_Last
    };
public:
    inline		CXlibMode (void)		: CMode() {}
    inline		CXlibMode (const CMode& v)	: CMode (v) {}
    inline const CMode&	operator= (const CMode& v)	{ CMode::operator= (v); return (*this); }
    void		WriteToX (XF86VidModeModeInfo& vi) const;
    void		ReadFromX (const XF86VidModeModeInfo& i);
};

} // namespace fbgl
