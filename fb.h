// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "gc.h"
#include "pal.h"
#include <X11/keysym.h>
#include <X11/Xlib.h>

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
				CXlibFramebuffer (void);
    inline			~CXlibFramebuffer (void) { Close(); }
    void			Open (void);
    void			Close (void);
    void			SetStandardMode (EStdFbMode m = stdmode_320x240x8, size_t freq = 60);
    void			CheckEvents (CApp* evp);
    void			Flush (void);
    inline const CGC&		GC (void) const	{ return (m_GC); }
    inline CGC&			GC (void)	{ return (m_GC); }
private:
    typedef ::GC		XGC;
private:
    void			CloseWindow (void);
    void			WaitForEvents (void);
    template <typename PixelType>
    void			InitColormap (PixelType* cmap) const;
    template <typename PixelType>
    void			CopyGCToImage (void);
private:
    CGC				m_GC;
    Display*			m_pDisplay;
    Visual*			m_pVisual;
    XGC				m_XGC;
    memblock			m_ImageData;
    XImage*			m_pImage;
    Window			m_Window;
};
