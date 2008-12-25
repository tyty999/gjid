// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// fb.h
//

#ifndef FB_H_7338CBBF13B535483C54DF471B2E407C
#define FB_H_7338CBBF13B535483C54DF471B2E407C

#include "mode.h"
#include "evp.h"
#include "gob/gc.h"

namespace fbgl {

enum EStdFbMode {
    stdmode_Text,
    stdmode_320x240x8,
    stdmode_640x480x8,
    stdmode_Last
};

/// \class CFramebuffer fb.h fbgl/fb.h
/// \brief Abstract framebuffer interface.
class CFramebuffer {
public:
    typedef const CMode&	rcmode_t;
public:
    virtual void		Open (void);
    virtual void		Close (void) = 0;
    virtual void		CheckEvents (CEventProcessor* evp) = 0;
    virtual void		Flush (void) = 0;
    virtual void		OnFocus (bool bFocus);
    virtual void		SetMode (rcmode_t m, size_t depth = 8);
    virtual void		SetStandardMode (EStdFbMode m = stdmode_320x240x8, size_t freq = 60);
    rcmode_t			FindClosestMode (size_t w, size_t h, size_t freq) const;
    inline const CGC&		GC (void) const	{ return (m_GC); }
    inline CGC&			GC (void)	{ return (m_GC); }
protected:
    typedef vector<CMode>	modevec_t;
protected:
				CFramebuffer (void);
    virtual		       ~CFramebuffer (void);
    virtual void		LoadModes (modevec_t& mv) = 0;
    inline void			SetOrigMode (rcmode_t m)	{ m_OrigMode = m; }
    inline rcmode_t		OrigMode (void) const		{ return (m_OrigMode); }
    inline void			SetCurMode (rcmode_t m)		{ m_CurMode = m; }
    inline rcmode_t		CurMode (void) const		{ return (m_CurMode); }
private:
    modevec_t			m_Modes;
    CMode			m_OrigMode;
    CMode			m_CurMode;
    CGC				m_GC;
};

} // namespace fbgl

#endif

