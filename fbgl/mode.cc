// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
//
// mode.cc
//

#include "mode.h"

namespace fbgl {

//----------------------------------------------------------------------

const char CMode::s_FlagText [flag_Last][12] = {
    "hsync high",	// flag_HSyncHigh
    "vsync high",	// flag_VSyncHigh
    "csync high",	// flag_CSync
    "extsyn true",	// flag_ExtSync
    "laced true",	// flag_Interlaced
    "double true",	// flag_Doublescan
    "gsync true",	// flag_SyncOnGreen
    "bcast true"	// flag_Broadcast
};
const CMode CMode::null_Mode;

//----------------------------------------------------------------------

/// Default constructor.
CMode::CMode (void)
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
CMode::CMode (const CMode& m)
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
CMode::~CMode (void)
{
}

/// Assignment operator.
const CMode& CMode::operator= (const CMode& m)
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
bool CMode::operator== (const CMode& m) const
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
size_t CMode::RefreshRate (void) const
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
void CMode::text_write (ostringstream& os) const
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

} // namespace fbgl

