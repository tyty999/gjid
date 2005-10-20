// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// mode.cc
//

#include "mode.h"
#include <linux/fb.h>

namespace fbgl {

/// Writes in mode values into \p vi.
void CConsoleMode::CreateVarinfo (struct fb_var_screeninfo& vi) const
{
    memset (&vi, 0, sizeof(vi));
    vi.xres = m_Width;
    vi.yres = m_Height;
    vi.xres_virtual = m_VWidth;
    vi.yres_virtual = m_VHeight;
    vi.xoffset = 0;
    vi.yoffset = 0;

    vi.bits_per_pixel = m_Depth;
    vi.grayscale = 0;
    static const uint8_t rays[] = {
	0, 0, 0, 0,
	2, 3, 3, 0,
	5, 6, 5, 0,
	8, 8, 8, 0,
	8, 8, 8, 8,
	0, 0, 0, 0,
	16, 16, 16, 0,
	 0,  0,  0, 0,
	16, 16, 16, 16
    };
    const size_t roff = m_Depth / 8 * 4;
    vi.red.length = rays[roff];
    vi.green.length = rays[roff + 1];
    vi.blue.length = rays[roff + 2];
    vi.transp.length = rays[roff + 3];
    vi.red.offset = 0;
    vi.green.offset = vi.red.offset + vi.red.length;
    vi.blue.offset = vi.green.offset + vi.green.length;
    vi.transp.offset = vi.blue.offset + vi.blue.length;

    vi.activate = FB_ACTIVATE_NOW;
    vi.pixclock = m_PixClock;
    vi.left_margin = m_LeftMargin;
    vi.right_margin = m_RightMargin;
    vi.upper_margin = m_UpperMargin;
    vi.lower_margin = m_LowerMargin;
    vi.hsync_len = m_HSyncLen;
    vi.vsync_len = m_VSyncLen;
    #define set_vi_flag(flag, var, value) if (Flag(flag)) vi.var |= value
    set_vi_flag (flag_HSyncHigh, sync, FB_SYNC_HOR_HIGH_ACT);
    set_vi_flag (flag_VSyncHigh, sync, FB_SYNC_VERT_HIGH_ACT);
    set_vi_flag (flag_CSync, sync, FB_SYNC_COMP_HIGH_ACT);
    set_vi_flag (flag_ExtSync, sync, FB_SYNC_EXT);
    set_vi_flag (flag_SyncOnGreen, sync, FB_SYNC_ON_GREEN);
    set_vi_flag (flag_Broadcast, sync, FB_SYNC_BROADCAST);
    #undef set_vi_flag
    if (Flag (flag_Interlaced)) vi.vmode = FB_VMODE_INTERLACED;
    else if (Flag (flag_Doublescan)) vi.vmode = FB_VMODE_DOUBLE;
    else vi.vmode = FB_VMODE_NONINTERLACED;
    vi.vmode |= FB_VMODE_CONUPDATE;
}

/// Reads mode information in modedb format.
CMode::mdbiter_t CConsoleMode::ReadFromModedb (const string& s)
{
    #define find_end_of(s)	find(s,i) + VectorSize(s)
    #define get_number(i)	strtoul (i, const_cast<char**>(&i), 10)
    mdbiter_t i = s.begin();
    i = s.find_end_of ("mode");
    if (i >= s.end()) return (s.end());
    mdbiter_t iend = s.find_end_of ("endmode");
    if (iend > s.end()) return (s.end());
    i = s.find ('\"', i) + 1;
    if (i >= iend) return (iend);
    m_Name.assign (i, s.find ('\"', i));
    i = s.find_end_of ("geometry");
    if (i >= iend) return (iend);
    m_Width = get_number (i);
    m_Height = get_number (i);
    m_VWidth = get_number (i);
    m_VHeight = get_number (i);
    m_Depth = get_number (i);
    i = s.find_end_of ("timings");
    if (i >= iend) return (iend);
    m_PixClock = get_number (i);
    m_LeftMargin = get_number (i);
    m_RightMargin = get_number (i);
    m_UpperMargin = get_number (i);
    m_LowerMargin = get_number (i);
    m_HSyncLen = get_number (i);
    m_VSyncLen = get_number (i);
    for (uoff_t f = 0; f < flag_Last; ++f)
	SetFlag (EFlag(f), s.find (s_FlagText[f]) < iend);
    return (iend);
    #undef get_number
    #undef find_end_of
}

} // namespace fbgl

