// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "fb.h"

/// Base class for application objects
class CApp {
public:
    typedef wchar_t		key_t;		///< Used for keycodes.
    typedef uint32_t		bidx_t;		///< Mouse button index.
public:
    inline void			Quit (void)	{ OnQuit(); }
    void			Update (void);
    int				Run (void);
protected:
    friend class CXlibFramebuffer;
				CApp (void);
    inline virtual void		OnIdle (void)	{ }
    inline virtual void		OnDraw (CGC&)	{ }
    inline virtual void		OnQuit (void)	{ _wantQuit = true; }
    inline virtual void		OnKey (key_t)	{ }
    inline virtual void		OnKeyUp (key_t)	{ }
    inline virtual void		OnMouseMove (coord_t, coord_t) {}
    inline virtual void		OnButton (bidx_t, coord_t, coord_t) {}
    inline virtual void		OnButtonUp (bidx_t, coord_t, coord_t) {}
    inline void			CreateWindow (const char* title, coord_t w, coord_t h) { _fb.CreateWindow (title, w, h); }
private:
    CXlibFramebuffer		_fb;		///< The framebuffer backend.
    CGC				_gc;		///< GC for drawing onto the offscreen buffer.
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
