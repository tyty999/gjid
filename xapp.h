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

/// Base class for application objects
class CXApp {
public:
    typedef wchar_t		key_t;		///< Used for keycodes.
    typedef uint32_t		bidx_t;		///< Mouse button index.
    typedef ::GC		XGC;
public:
    inline void			Quit (void)	{ OnQuit(); }
    void			Update (void);
    int				Run (void);
protected:
				CXApp (void);
    virtual			~CXApp (void);
    inline virtual void		OnIdle (void)	{ }
    inline virtual void		OnDraw (CGC&)	{ }
    inline virtual void		OnQuit (void)	{ _wantQuit = true; }
    inline virtual void		OnKey (key_t)	{ }
    inline virtual void		OnKeyUp (key_t)	{ }
    inline virtual void		OnMouseMove (coord_t, coord_t) {}
    inline virtual void		OnButton (bidx_t, coord_t, coord_t) {}
    inline virtual void		OnButtonUp (bidx_t, coord_t, coord_t) {}
    void			CreateWindow (const char* title, coord_t w, coord_t h);
private:
    void			CheckEvents (CXApp* evp);
    void			Flush (void);
    inline const CGC&		GC (void) const	{ return (_gc); }
    inline CGC&			GC (void)	{ return (_gc); }
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
    bool			_wantQuit;	///< True if want to quit
};

//----------------------------------------------------------------------

extern "C" void InstallCleanupHandlers (void);

template <typename AppClass>
inline int TMainApp (int, const char* const*)
{
    int rv = EXIT_FAILURE;
    try {
	rv = AppClass::Instance().Run();
    } catch (exception& e) {
	printf ("Error: %s\n", e.what());
    }
    return (rv);
}

//----------------------------------------------------------------------

#define MainApp(appClass)				\
    int main (int argc, const char* const* argv)	\
	{ return (TMainApp<appClass> (argc, argv)); }

//----------------------------------------------------------------------
