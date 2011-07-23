// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "mode.h"

namespace fbgl {

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

} // namespace fbgl
