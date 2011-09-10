// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "gc.h"
#include "pal.h"
#include <xcb/xcb.h>
#include <X11/keysym.h>

//----------------------------------------------------------------------

enum {
    _XKM_Bitshift = 24,
    XKM_Shift = 1<<_XKM_Bitshift,
    XKM_Ctrl = 4<<_XKM_Bitshift,
    XKM_Alt = 8<<_XKM_Bitshift,
    XKM_Mask = XKM_Shift| XKM_Ctrl| XKM_Alt
};

/// Base class for application objects
class CXApp {
public:
    typedef wchar_t		key_t;		///< Used for keycodes.
    typedef uint32_t		bidx_t;		///< Mouse button index.
private:
    enum EXRFmt {
	rfmt_Default,
	rfmt_Bitmask,
	rfmt_Font,
	rfmt_Pixmap
    };
    enum EXAtoms {
	xa_STRING,
	xa_ATOM,
	xa_WM_NAME,
	xa_WM_PROTOCOLS,
	xa_WM_DELETE_WINDOW,
	xa_NET_WM_STATE,
	xa_NET_WM_STATE_FULLSCREEN,
	xa_Count
    };
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
    void			CreateWindow (const char* title, coord_t w, coord_t h);
private:
    inline const CGC&		GC (void) const	{ return (_gc); }
    inline CGC&			GC (void)	{ return (_gc); }
    inline void			OnMap (void);
    inline void			OnResize (const xcb_generic_event_t* event);
    inline wchar_t		TranslateKeycode (const xcb_generic_event_t* event) const;
private:
    CGC				_gc;
    vector<wchar_t>		_ksyms;
    xcb_connection_t*		_pconn;
    const xcb_screen_t*		_pscreen;
    xcb_window_t		_window;
    uint32_t			_wpict;
    uint32_t			_bpict;
    uint32_t			_bpixid;
    xcb_gcontext_t		_xgc;
    xcb_atom_t			_atoms[xa_Count];
    xcb_atom_t			_xa_wm_protocols;
    xcb_atom_t			_xa_wm_delete_window;
    xcb_atom_t			_xa_net_wm_state;
    xcb_atom_t			_xa_net_wm_state_fullscreen;
    uint8_t			_xrfmt[4];
    uint16_t			_width;
    uint16_t			_height;
    uint8_t			_minKeycode;
    uint8_t			_keysymsPerKeycode;
    bool			_wantQuit;
};

//----------------------------------------------------------------------

extern "C" void InstallCleanupHandlers (void);

template <typename AppClass>
inline int TMainApp (int, const char* const*)
{
    try {
	return (AppClass::Instance().Run());
    } catch (exception& e) {
	printf ("Error: %s\n", e.what());
    }
    return (EXIT_FAILURE);
}

//----------------------------------------------------------------------

#define MainApp(appClass)				\
    int main (int argc, const char* const* argv)	\
	{ return (TMainApp<appClass> (argc, argv)); }

//----------------------------------------------------------------------
