// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "gc.h"
#include "pal.h"
#include <X11/keysym.h>
#include <X11/Xlib.h>

//----------------------------------------------------------------------

enum {
    _XKM_Bitshift = 24,
    XKM_Shift = ShiftMask << _XKM_Bitshift,
    XKM_Ctrl = ControlMask << _XKM_Bitshift,
    XKM_Alt = Mod1Mask << _XKM_Bitshift,
    XKM_Mask = XKM_Shift| XKM_Ctrl| XKM_Alt
};

class CApp;

//----------------------------------------------------------------------

class CXlibFramebuffer {
public:
				CXlibFramebuffer (void);
    inline			~CXlibFramebuffer (void) { Close(); }
    void			Open (void);
    void			Close (void);
    void			CreateWindow (const char* title, coord_t w, coord_t h);
    void			CheckEvents (CApp* evp);
    void			Flush (void);
    inline const CGC&		GC (void) const	{ return (_gc); }
    inline CGC&			GC (void)	{ return (_gc); }
private:
    typedef ::GC		XGC;
private:
    void			CloseWindow (void);
    void			WaitForEvents (void);
    inline void			OnMap (void);
    inline void			OnConfigure (coord_t width, coord_t height);
    template <typename PixelType>
    void			InitColormap (PixelType* cmap) const;
    template <typename PixelType>
    void			CopyGCToImage (void);
private:
    CGC				_gc;
    Display*			_pDisplay;
    Visual*			_pVisual;
    XGC				_xgc;
    memblock			_imageData;
    XImage*			_pImage;
    Window			_window;
};
