// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "config.h"
#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>

class CXlibMode {
public:
    enum EFlag {
	flag_HSyncHigh,
	flag_VSyncHigh,
	flag_CSync,
	flag_ExtSync,
	flag_Interlaced,
	flag_Doublescan,
	flag_SyncOnGreen,
	flag_Broadcast,
	flag_Last
    };
    enum XModeFlagBits {		/// Bits for flags in XF86VidModeModeInfo
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
    typedef string::const_iterator	mdbiter_t;
    typedef const string&		rcname_t;
    static const CXlibMode			null_Mode;
public:
			CXlibMode (void);
			CXlibMode (const CXlibMode& m);
		       ~CXlibMode (void);
    const CXlibMode&	operator= (const CXlibMode& m);
    bool		operator== (const CXlibMode& m) const;
    void		text_write (ostringstream& os) const;
    inline rcname_t	Name (void) const	{ return (m_Name); }
    inline size_t	Width (void) const	{ return (m_Width); }
    inline size_t	Height (void) const	{ return (m_Height); }
    inline size_t	Depth (void) const	{ return (m_Depth); }
    inline void		SetDepth (size_t v)	{ m_Depth = v; }
    inline bool		Flag (EFlag f) const	{ return (m_Flags & (1 << f)); }
    size_t		RefreshRate (void) const;
    void		WriteToX (XF86VidModeModeInfo& vi) const;
    void		ReadFromX (const XF86VidModeModeInfo& i);
protected:
    inline void		SetFlag (EFlag f, bool v = true);
protected:
    string		m_Name;		///< Mode name; as in "800x600-16@75"
    uint32_t		m_PixClock;	///< Clock frequency in MHz
    uint16_t		m_LeftMargin;	///< Time in us from hsync to picture
    uint16_t		m_RightMargin;	///< Time in us from picture to hsync
    uint16_t		m_UpperMargin;	///< Time in us from vsync to picture
    uint16_t		m_LowerMargin;	///< Time in us from picture to vsync
    uint16_t		m_HSyncLen;	///< Time in us of hsync
    uint16_t		m_VSyncLen;	///< Time in us of vsync
    uint16_t		m_Width;	///< X resolution in pixels.
    uint16_t		m_Height;	///< Y resolution in pixels.
    uint16_t		m_VWidth;	///< Virtual X resolution in pixels.
    uint16_t		m_VHeight;	///< Virtual Y resolution in pixels.
    uint16_t		m_Depth;	///< Color depth in bits.
    uint16_t		m_Flags;	///< See EFlags for values.
    static const char	s_FlagText [flag_Last][12];
};

//----------------------------------------------------------------------

/// Sets flag \p f to \p v.
inline void CXlibMode::SetFlag (EFlag f, bool v)
{
    const uint16_t mask = 1 << f;
    if (v)
	m_Flags |= mask;
    else
	m_Flags &= ~mask;
}

//----------------------------------------------------------------------

TEXT_STREAMABLE(CXlibMode)
