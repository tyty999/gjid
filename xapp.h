// Copyright (c) 1995 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "config.h"
#include <X11/keysym.h>

//----------------------------------------------------------------------

enum {
    _XKM_Bitshift = 24,
    XKM_Shift = 1<<_XKM_Bitshift,
    XKM_Ctrl = 4<<_XKM_Bitshift,
    XKM_Alt = 8<<_XKM_Bitshift,
    XKM_Mask = XKM_Shift| XKM_Ctrl| XKM_Alt
};

struct xcb_connection_t;
struct xcb_screen_t;

/// Base class for application objects
class CXApp {
public:
    typedef wchar_t		key_t;		///< Used for keycodes.
    typedef uint32_t		bidx_t;		///< Mouse button index.
    struct SImage {
	uint32_t		id;
	uint16_t		w,h;
    };
    struct SImageTile {
	uint8_t			x,y,w,h;
    };
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
    inline virtual void		OnDraw (void)	{ }
    inline virtual void		OnQuit (void)	{ _wantQuit = true; }
    inline virtual void		OnKey (key_t)	{ }
    inline uint16_t		Width (void) const				{ return (_width); }
    inline uint16_t		Height (void) const				{ return (_height); }
    inline uint32_t		RGB (uint8_t r, uint8_t g, uint8_t b) const	{ return (r<<16|g<<8|b); }
    SImage			LoadImage (const char* const* p);
    void			DrawImageTile (const SImage& img, const SImageTile& tile, int x, int y);
    void			CreateWindow (const char* title, int w, int h);
    void			DrawText (int x, int y, const char* s, uint32_t color);
private:
    inline void			OnMap (void);
    inline void			OnResize (const void* event);
    inline wchar_t		TranslateKeycode (const void* event) const;
    inline void			OnClientMessage (const void* e);
    void			LoadFont (void);
private:
    vector<wchar_t>		_ksyms;
    xcb_connection_t*		_pconn;
    const xcb_screen_t*		_pscreen;
    uint32_t			_window;
    uint32_t			_wpict;
    uint32_t			_bpict;
    uint32_t			_glyphset;
    uint32_t			_glyphpen;
    uint32_t			_pencolor;
    uint32_t			_xgc;
    uint32_t			_atoms[xa_Count];
    uint16_t			_xrfmt[4];
    uint16_t			_width;
    uint16_t			_height;
    uint16_t			_winWidth;
    uint16_t			_winHeight;
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
