// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "mode.h"

//----------------------------------------------------------------------

const char CXlibMode::s_FlagText [flag_Last][12] = {
    "hsync high",	// flag_HSyncHigh
    "vsync high",	// flag_VSyncHigh
    "csync high",	// flag_CSync
    "extsyn true",	// flag_ExtSync
    "laced true",	// flag_Interlaced
    "double true",	// flag_Doublescan
    "gsync true",	// flag_SyncOnGreen
    "bcast true"	// flag_Broadcast
};
const CXlibMode CXlibMode::null_Mode;

//----------------------------------------------------------------------

/// Default constructor.
CXlibMode::CXlibMode (void)
: m_Name (),
  m_PixClock (0),
  m_LeftMargin (0),
  m_RightMargin (0),
  m_UpperMargin (0),
  m_LowerMargin (0),
  m_HSyncLen (0),
  m_VSyncLen (0),
  m_Width (0),
  m_Height (0),
  m_VWidth (0),
  m_VHeight (0),
  m_Depth (0),
  m_Flags (0)
{
}

/// Copy constructor.
CXlibMode::CXlibMode (const CXlibMode& m)
: m_Name (m.m_Name),
  m_PixClock (m.m_PixClock),
  m_LeftMargin (m.m_LeftMargin),
  m_RightMargin (m.m_RightMargin),
  m_UpperMargin (m.m_UpperMargin),
  m_LowerMargin (m.m_LowerMargin),
  m_HSyncLen (m.m_HSyncLen),
  m_VSyncLen (m.m_VSyncLen),
  m_Width (m.m_Width),
  m_Height (m.m_Height),
  m_VWidth (m.m_VWidth),
  m_VHeight (m.m_VHeight),
  m_Depth (m.m_Depth),
  m_Flags (m.m_Flags)
{
}

/// Destructor instantiated to avoid inlining.
CXlibMode::~CXlibMode (void)
{
}

/// Assignment operator.
const CXlibMode& CXlibMode::operator= (const CXlibMode& m)
{
    m_Name = m.m_Name;
    m_PixClock = m.m_PixClock;
    m_LeftMargin = m.m_LeftMargin;
    m_RightMargin = m.m_RightMargin;
    m_UpperMargin = m.m_UpperMargin;
    m_LowerMargin = m.m_LowerMargin;
    m_HSyncLen = m.m_HSyncLen;
    m_VSyncLen = m.m_VSyncLen;
    m_Width = m.m_Width;
    m_Height = m.m_Height;
    m_VWidth = m.m_VWidth;
    m_VHeight = m.m_VHeight;
    m_Depth = m.m_Depth;
    m_Flags = m.m_Flags;
    return (*this);
}

/// Returns true if equal to \p m.
bool CXlibMode::operator== (const CXlibMode& m) const
{
    return (m_PixClock == m.m_PixClock &&
	    m_LeftMargin == m.m_LeftMargin &&
	    m_RightMargin == m.m_RightMargin &&
	    m_UpperMargin == m.m_UpperMargin &&
	    m_LowerMargin == m.m_LowerMargin &&
	    m_HSyncLen == m.m_HSyncLen &&
	    m_VSyncLen == m.m_VSyncLen &&
	    m_Width == m.m_Width &&
	    m_Height == m.m_Height &&
	    m_VWidth == m.m_VWidth &&
	    m_VHeight == m.m_VHeight &&
	    m_Depth == m.m_Depth &&
	    m_Flags == m.m_Flags);
}

/// Returns the frame refresh rate for this mode in Hz.
size_t CXlibMode::RefreshRate (void) const
{
    const size_t htotal = m_LeftMargin + m_Width + m_RightMargin + m_HSyncLen;
    size_t vtotal = m_UpperMargin + m_Height + m_LowerMargin + m_VSyncLen;
    if (!Flag (flag_Interlaced))
	vtotal *= 2;
    else if (Flag (flag_Doublescan))
	vtotal *= 4;
    return (size_t(((1E12f / (m_PixClock ? m_PixClock : 1)) / htotal) / vtotal * 2));
}

/// Writes information about the this mode.
void CXlibMode::text_write (ostringstream& os) const
{
    os.format ( "mode \"%s\"\n"
		"    # VRate %zu Hz\n"
		"    geometry %u %u %u %u %u\n"
		"    timings %u %u %u %u %u %u %u\n",
	    m_Name.c_str(), RefreshRate(), m_Width, m_Height, m_VWidth, m_VHeight, m_Depth,
	    m_PixClock, m_LeftMargin, m_RightMargin, m_UpperMargin, m_LowerMargin, m_HSyncLen, m_VSyncLen);
    for (uoff_t f = 0; f < flag_Last; ++ f)
	if (Flag(EFlag(f))) os.format ("    %s\n", s_FlagText[f]);
    os << "endmode\n";
}

/// Writes in mode values into \p vi.
void CXlibMode::WriteToX (XF86VidModeModeInfo& vi) const
{
    vi.dotclock = m_PixClock;
    vi.hdisplay = m_Width;
    vi.hsyncstart = vi.hdisplay + m_RightMargin;
    vi.hsyncend = vi.hsyncstart + m_HSyncLen;
    vi.htotal = vi.hsyncend + m_LeftMargin;
    vi.hskew = 0;
    vi.vdisplay = m_Height;
    vi.vsyncstart = vi.vdisplay + m_LowerMargin;
    vi.vsyncend = vi.vsyncstart + m_VSyncLen;
    vi.vtotal = vi.vsyncend + m_UpperMargin;
    vi.flags = 0;
    vi.privsize = 0;
    vi.c_private = NULL;
    #define SET_XFLAG(flag)				vi.flags |= (1 << (flag))
    #define MAP_NEGPOS_FLAG(flag, posxflag, negxflag)	SET_XFLAG (Flag (flag) ? posxflag : negxflag)
    #define MAP_FLAG(flag, xflag)			if (Flag(flag)) SET_XFLAG(xflag)
    MAP_NEGPOS_FLAG (flag_HSyncHigh, xflag_PosHSync, xflag_NegHSync);
    MAP_NEGPOS_FLAG (flag_VSyncHigh, xflag_PosVSync, xflag_NegVSync);
    MAP_FLAG (flag_CSync, xflag_CSync);
    MAP_FLAG (flag_Interlaced, xflag_Interlaced);
    MAP_FLAG (flag_Doublescan, xflag_Doublescan);
    #undef MAP_NEGPOS_FLAG
    #undef MAP_FLAG
    #undef SET_XFLAG
}

void CXlibMode::ReadFromX (const XF86VidModeModeInfo& vi)
{
    m_PixClock = vi.dotclock;
    m_Width = vi.hdisplay;
    m_LeftMargin = vi.htotal - vi.hsyncend;
    m_RightMargin = vi.hsyncstart - vi.hdisplay;
    m_HSyncLen = vi.hsyncend - vi.hsyncstart;
    m_Height = vi.vdisplay;
    m_LowerMargin = vi.vsyncstart - vi.vdisplay;
    m_UpperMargin = vi.vtotal - vi.vsyncend;
    m_VSyncLen = vi.vsyncend - vi.vsyncstart;
    m_VWidth = m_Width;
    m_VHeight = m_Height;
    m_Depth = 8;
    m_Flags = 0;
    m_Name.format ("%ux%u-%zu", m_Width, m_Height, RefreshRate());
    #define MAP_FLAG(xflag,flag)	if (vi.flags & (1 << xflag)) SetFlag (flag)
    MAP_FLAG (xflag_PosHSync,	flag_HSyncHigh);
    MAP_FLAG (xflag_PosVSync,	flag_VSyncHigh);
    MAP_FLAG (xflag_CSync,	flag_CSync);
    MAP_FLAG (xflag_Interlaced,	flag_Interlaced);
    MAP_FLAG (xflag_Doublescan,	flag_Doublescan);
    #undef MAP_FLAG
}
