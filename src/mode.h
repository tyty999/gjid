// Copyright (c) 2003-2006 by Mike Sharov <msharov@users.sourceforge.net>
//
// fbmode.h
//

#ifndef FBMODE_H_305B7A305F04122E318379A84269939F
#define FBMODE_H_305B7A305F04122E318379A84269939F

#include "file.h"

struct fb_var_screeninfo;

namespace fbgl {

/// \class CFbMode fbmode.h "fbmode.h"
///
/// \brief Stores information about a framebuffer mode from fb.modes
///
class CFbMode {
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
    typedef string::const_iterator	mdbiter_t;
    typedef const string&		rcname_t;
public:
			CFbMode (void);
    void		CreateVarinfo (struct fb_var_screeninfo& vi) const;
    mdbiter_t		ReadFromModedb (const string& i);
    void		read (istream& is);
    void		write (ostream& os) const;
    size_t		stream_size (void) const;
    void		text_write (ostringstream& os) const;
    inline rcname_t	Name (void) const	{ return (m_Name); }
    inline size_t	Width (void) const	{ return (m_Width); }
    inline size_t	Height (void) const	{ return (m_Height); }
    inline size_t	Depth (void) const	{ return (m_Depth); }
    inline void		SetDepth (size_t v)	{ m_Depth = v; }
    inline bool		Flag (EFlag f) const	{ return (m_Flags & (1 << f)); }
    size_t		RefreshRate (void) const;
private:
    inline void		SetFlag (EFlag f, bool v = true);
private:
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
    static const char*	s_FlagText [flag_Last];
};

} // namespace fbgl

TEXT_STREAMABLE(fbgl::CFbMode)

#endif

