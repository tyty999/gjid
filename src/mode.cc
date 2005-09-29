// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
//
// fbmode.h
//

#include "fbmode.h"
#include <linux/fb.h>

namespace fbgl {

//----------------------------------------------------------------------

const char* CFbMode::s_FlagText [flag_Last] = {
    "hsync high",	// flag_HSyncHigh
    "vsync high",	// flag_VSyncHigh
    "csync high",	// flag_CSync
    "extsync true",	// flag_ExtSync
    "laced true",	// flag_Interlaced
    "double true",	// flag_Doublescan
    "gsync true",	// flag_SyncOnGreen
    "bcast true"	// flag_Broadcast
};

//----------------------------------------------------------------------

/// Default constructor.
CFbMode::CFbMode (void)
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

/// Sets flag \p f to \p v.
inline void CFbMode::SetFlag (EFlag f, bool v)
{
    const uint16_t mask = 1 << f;
    if (v)
	m_Flags |= mask;
    else
	m_Flags &= ~mask;
}

/// Writes in mode values into \p vi.
void CFbMode::CreateVarinfo (struct fb_var_screeninfo& vi) const
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
CFbMode::mdbiter_t CFbMode::ReadFromModedb (const string& s)
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

/// Returns the frame refresh rate for this mode in Hz.
size_t CFbMode::RefreshRate (void) const
{
    const size_t htotal = m_LeftMargin + m_Width + m_RightMargin + m_HSyncLen;
    size_t vtotal = m_UpperMargin + m_Height + m_LowerMargin + m_VSyncLen;
    if (!Flag (flag_Interlaced))
	vtotal *= 2;
    else if (Flag (flag_Doublescan))
	vtotal *= 4;
    return (size_t(((1E12f / (m_PixClock ? m_PixClock : 1)) / htotal) / vtotal * 2));
}

/// Reads the object from stream \p is.
void CFbMode::read (istream& is)
{
    is >> m_Name >> ios::talign<uint32_t>();
    is >> m_PixClock;
    is >> m_LeftMargin >> m_RightMargin >> m_UpperMargin >> m_LowerMargin;
    is >> m_HSyncLen >> m_VSyncLen;
    is >> m_Width >> m_Height >> m_VWidth >> m_VHeight >> m_Depth;
    is >> m_Flags >> ios::align();
}

/// Writes the object to stream \p os.
void CFbMode::write (ostream& os) const
{
    os << m_Name << ios::talign<uint32_t>();
    os << m_PixClock;
    os << m_LeftMargin << m_RightMargin << m_UpperMargin << m_LowerMargin;
    os << m_HSyncLen << m_VSyncLen;
    os << m_Width << m_Height << m_VWidth << m_VHeight << m_Depth;
    os << m_Flags << ios::align();
}

/// Returns the size of the written object.
size_t CFbMode::stream_size (void) const
{
    return (Align (
		Align (stream_size_of(m_Name), alignof(m_PixClock)) +
		stream_size_of (m_PixClock) +
		stream_size_of (m_LeftMargin) +
		stream_size_of (m_RightMargin) +
		stream_size_of (m_UpperMargin) +
		stream_size_of (m_LowerMargin) +
		stream_size_of (m_HSyncLen) +
		stream_size_of (m_VSyncLen) +
		stream_size_of (m_Width) +
		stream_size_of (m_Height) +
		stream_size_of (m_VWidth) +
		stream_size_of (m_VHeight) +
		stream_size_of (m_Depth) +
		stream_size_of (m_Flags)));
}

/// Writes information about the this mode.
void CFbMode::text_write (ostringstream& os) const
{
    os << "mode \"" << m_Name << "\"\n";
    os.format ("    # VRate %u Hz\n", RefreshRate());
    os.format ("    geometry %u %u %u %u %u\n", m_Width, m_Height, m_VWidth, m_VHeight, m_Depth);
    os.format ("    timings %u %u %u %u %u %u %u\n", m_PixClock, m_LeftMargin, m_RightMargin, m_UpperMargin, m_LowerMargin, m_HSyncLen, m_VSyncLen);
    for (uoff_t f = 0; f < flag_Last; ++ f)
	if (Flag(EFlag(f))) os.format ("    %s\n", s_FlagText[f]);
    os << "endmode\n";
}

} // namespace fbgl

