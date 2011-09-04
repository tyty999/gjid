// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "xapp.h"
#include <xcb/xcb_atom.h>
#include <xcb/render.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

//----------------------------------------------------------------------

/// Called when a signal is received.
static void OnSignal (int sig)
{
    printf ("[S] Fatal error: %s\n", strsignal(sig));
    exit (-sig);
}

/// Called by the framework on unrecoverable exception handling errors.
static void Terminate (void)
{
    printf ("[T] Unexpected fatal error\n");
    exit (EXIT_FAILURE);
}

//----------------------------------------------------------------------

CXApp::CXApp (void)
:_gc()
,_ksyms()
,_pconn (NULL)
,_pscreen (NULL)
,_window (XCB_NONE)
,_xgc (XCB_NONE)
,_width()
,_height()
,_minKeycode()
,_keysymsPerKeycode()
,_wantQuit (false)
{
    // Initialize cleanup handlers
    static const int8_t c_Signals[] = {
	SIGILL, SIGABRT, SIGBUS,  SIGFPE,  SIGSEGV, SIGSYS, SIGALRM, SIGXCPU,
	SIGHUP, SIGINT,  SIGQUIT, SIGTERM, SIGCHLD, SIGXFSZ, SIGPWR, SIGPIPE
    };
    for (uoff_t i = 0; i < VectorSize(c_Signals); ++ i)
	signal (c_Signals[i], OnSignal);
    std::set_terminate (Terminate);

    // Establish X server connection
    _pconn = xcb_connect (NULL, NULL);
    const xcb_setup_t* xsetup = xcb_get_setup (_pconn);
    _pscreen = xcb_setup_roots_iterator(xsetup).data;
    auto kbcookie = xcb_get_keyboard_mapping (_pconn, xsetup->min_keycode, xsetup->max_keycode-xsetup->min_keycode);
    auto rendcook = xcb_render_query_version (_pconn, XCB_RENDER_MAJOR_VERSION, XCB_RENDER_MINOR_VERSION);
    auto r_wm_protocols = xcb_intern_atom (_pconn, false, strlen("WM_NAME"), "WM_NAME");
    auto r_wm_delete_window = xcb_intern_atom (_pconn, false, strlen("WM_NAME"), "WM_NAME");
    auto r_net_wm_state = xcb_intern_atom (_pconn, false, strlen("WM_NAME"), "WM_NAME");
    auto r_net_wm_state_fullscreen = xcb_intern_atom (_pconn, false, strlen("WM_NAME"), "WM_NAME");

    const xcb_get_keyboard_mapping_reply_t* kbreply = xcb_get_keyboard_mapping_reply (_pconn, kbcookie, NULL);
    size_t szkeysyms = xcb_get_keyboard_mapping_keysyms_length (kbreply);
    const wchar_t* psyms = (const wchar_t*) xcb_get_keyboard_mapping_keysyms (kbreply);
    _ksyms.assign (psyms, psyms + szkeysyms);
    _minKeycode = xsetup->min_keycode;
    _keysymsPerKeycode = kbreply->keysyms_per_keycode;

    xcb_render_query_version_reply (_pconn, rendcook, NULL);
    _xa_wm_protocols = xcb_intern_atom_reply(_pconn, r_wm_protocols, NULL)->atom;
    _xa_wm_delete_window = xcb_intern_atom_reply(_pconn, r_wm_delete_window, NULL)->atom;
    _xa_net_wm_state = xcb_intern_atom_reply(_pconn, r_net_wm_state, NULL)->atom;
    _xa_net_wm_state_fullscreen = xcb_intern_atom_reply(_pconn, r_net_wm_state_fullscreen, NULL)->atom;
}

/// Closes all active resources, windows, and server connections.
CXApp::~CXApp (void)
{
    if (!_pconn) return;
    xcb_disconnect (_pconn);
    _pconn = NULL;
}

int CXApp::Run (void)
{
    _wantQuit = false;
    xcb_flush(_pconn);
    for (xcb_generic_event_t* e; !_wantQuit && (e = xcb_wait_for_event(_pconn)); free(e)) {
	switch (e->response_type & 0x7f) {
	    case XCB_MAP_NOTIFY:	OnMap(); break;
	    case XCB_EXPOSE:		Update(); break;
	    case XCB_CONFIGURE_NOTIFY:
		_width = ((const xcb_configure_notify_event_t*)e)->width;
		_height = ((const xcb_configure_notify_event_t*)e)->height;
		break;
	    case XCB_KEY_PRESS:		OnKey (TranslateKeycode(e)); break;
	}
    }
    return (EXIT_SUCCESS);
}

void CXApp::Update (void)
{
    if (!_pconn || !_window || !GC().begin())
	return;
    OnDraw (GC());

    vector<uint32_t> img (_width*_height);
    CopyGCToImage (img);
    xcb_put_image (_pconn, XCB_IMAGE_FORMAT_Z_PIXMAP, _window, _xgc, _width, _height, 0, 0, 0, _pscreen->root_depth, img.size()*4, (const uint8_t*) &img[0]);
    xcb_flush(_pconn);
}

//----------------------------------------------------------------------
// Window and mode management
//----------------------------------------------------------------------

void CXApp::CreateWindow (const char* title, coord_t width, coord_t height)
{
    // Create the window with given dimensions
    static const uint32_t winvals[] = { XCB_NONE, XCB_EVENT_MASK_EXPOSURE| XCB_EVENT_MASK_KEY_PRESS| XCB_EVENT_MASK_STRUCTURE_NOTIFY };
    xcb_create_window (_pconn, XCB_COPY_FROM_PARENT, _window=xcb_generate_id(_pconn),
	    _pscreen->root, 0, 0, _width = width, _height = height, 0,
	    XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT,
	    XCB_CW_BACK_PIXMAP| XCB_CW_EVENT_MASK, winvals);
    // Create a GC for this window
    xcb_create_gc (_pconn, _xgc = xcb_generate_id(_pconn), _window, 0, NULL);
    // Set window title
    xcb_change_property (_pconn, XCB_PROP_MODE_REPLACE, _window, WM_NAME, STRING, 8, strlen(title), title);
    // Enable WM close message
    xcb_change_property (_pconn, XCB_PROP_MODE_REPLACE, _window, _xa_wm_protocols, ATOM, 32, 1, &_xa_wm_delete_window);
    // And put it on the screen
    xcb_map_window (_pconn, _window);

    // Initialize the palette to grayscale to avoid a black screen if the palette is not set.
    GC().Palette().resize (256);
    for (uoff_t i = 0; i < GC().Palette().size(); ++ i)
	GC().Palette()[i] = RGB (i, i, i);
    _gc.Resize (width, height);
}

inline void CXApp::OnMap (void)
{
    // Set the fullscreen flag on the window
    xcb_client_message_event_t xev;
    memset (&xev, 0, sizeof(xev));
    xev.response_type = XCB_CLIENT_MESSAGE;
    xev.window = _window;
    xev.type = _xa_net_wm_state;
    xev.format = 32;
    xev.data.data32[0] = 1;
    xev.data.data32[1] = _xa_net_wm_state_fullscreen;
    xcb_send_event (_pconn, _pscreen->root, false, XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT| XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY, (const char*) &xev);
    xcb_flush(_pconn);
}

inline wchar_t CXApp::TranslateKeycode (const xcb_generic_event_t* event) const
{
    const xcb_key_press_event_t *kp = (const xcb_key_press_event_t*)event;
    return (_ksyms[(kp->detail-_minKeycode)*_keysymsPerKeycode]);
}

//----------------------------------------------------------------------
// Drawing and colormap translation.
//----------------------------------------------------------------------

/// Copies GC contents to the image.
void CXApp::CopyGCToImage (vector<uint32_t>& img)
{
    const CPalette& rpal (GC().Palette());
    const color_t* src = GC().begin();
    auto dest = img.begin();

    // Scale the gc to the screen resolution.
    const size_t sw = GC().Width(), sh = GC().Height();
    const size_t dw = _width, dh = _height;
    size_t dx = 0, dy = 0;
    for (size_t y = 0; y < sh; ++y) {
	for (; dy < dh; dy += sh) {
	    for (size_t x = 0; x < sw; ++x) {
		const uint32_t v (rpal[src[x]]);
		for (; dx < dw; dx += sw)
		    *dest++ = v;
		dx -= dw;
	    }
	}
	dy -= dh;
	src += sw;
    }
}
