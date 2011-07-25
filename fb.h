// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "mode.h"
#include "gc.h"
#include "pal.h"
#include <X11/keysym.h>

//----------------------------------------------------------------------

enum EStdFbMode {
    stdmode_Text,
    stdmode_320x240x8,
    stdmode_640x480x8,
    stdmode_Last
};

enum {
    ModKeyReleasedMask = Mod4Mask,
    _XKM_Bitshift = 24,
    XKM_Shift = ShiftMask << _XKM_Bitshift,
    XKM_Ctrl = ControlMask << _XKM_Bitshift,
    XKM_Alt = Mod1Mask << _XKM_Bitshift,
    XKM_Released = ModKeyReleasedMask << _XKM_Bitshift,
    XKM_Mask = XKM_Shift| XKM_Ctrl| XKM_Alt| XKM_Released
};

class CApp;

//----------------------------------------------------------------------

class CXlibFramebuffer {
public:
    typedef const CXlibMode&	rcmode_t;
public:
				CXlibFramebuffer (void);
				~CXlibFramebuffer (void);
    void			Open (void);
    void			Close (void);
    void			SetMode (rcmode_t m);
    void			SetStandardMode (EStdFbMode m = stdmode_320x240x8, size_t freq = 60);
    rcmode_t			FindClosestMode (size_t w, size_t h, size_t freq) const;
    void			CheckEvents (CApp* evp);
    void			Flush (void);
    inline const CGC&		GC (void) const	{ return (m_GC); }
    inline CGC&			GC (void)	{ return (m_GC); }
private:
    typedef vector<CXlibMode>	modevec_t;
    typedef ::GC		XGC;
private:
    void			LoadModes (modevec_t& mv);
    inline void			SetOrigMode (rcmode_t m)	{ m_OrigMode = m; }
    inline rcmode_t		OrigMode (void) const		{ return (m_OrigMode); }
    inline void			SetCurMode (rcmode_t m)		{ m_CurMode = m; }
    inline rcmode_t		CurMode (void) const		{ return (m_CurMode); }
    void			SwitchToMode (const CXlibMode& nm);
    void			CloseWindow (void);
    void			WaitForEvents (void);
    void			SetFullscreenMode (bool v = true);
    template <typename PixelType>
    void			InitColormap (PixelType* cmap) const;
    template <typename PixelType>
    void			CopyGCToImage (void);
private:
    modevec_t			m_Modes;
    CXlibMode			m_OrigMode;
    CXlibMode			m_CurMode;
    CGC				m_GC;
    Display*			m_pDisplay;
    Visual*			m_pVisual;
    XGC				m_XGC;
    memblock			m_ImageData;
    XImage*			m_pImage;
    Window			m_Window;
};
