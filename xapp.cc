// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "xapp.h"
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
,_wpict (XCB_NONE)
,_bpict (XCB_NONE)
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
    // Request RENDER extension, keyboard mappings, and WM atoms
    xcb_get_keyboard_mapping_cookie_t kbcookie = xcb_get_keyboard_mapping (_pconn, xsetup->min_keycode, xsetup->max_keycode-xsetup->min_keycode);
    xcb_render_query_version_cookie_t rendcook = xcb_render_query_version (_pconn, XCB_RENDER_MAJOR_VERSION, XCB_RENDER_MINOR_VERSION);
    static const char* c_AtomNames[xa_Count] = { "STRING", "ATOM", "WM_NAME", "WM_PROTOCOLS", "WM_DELETE_WINDOW", "_NET_WM_STATE", "_NET_WM_STATE_FULLSCREEN" };
    for (int i = 0; i < xa_Count; ++i)
	_atoms[i] = xcb_intern_atom (_pconn, false, strlen(c_AtomNames[i]), c_AtomNames[i]).sequence;

    // Receive and store keyboard mappings
    const xcb_get_keyboard_mapping_reply_t* kbreply = xcb_get_keyboard_mapping_reply (_pconn, kbcookie, NULL);
    size_t szkeysyms = xcb_get_keyboard_mapping_keysyms_length (kbreply);
    const wchar_t* psyms = (const wchar_t*) xcb_get_keyboard_mapping_keysyms (kbreply);
    _ksyms.assign (psyms, psyms + szkeysyms);
    _minKeycode = xsetup->min_keycode;
    _keysymsPerKeycode = kbreply->keysyms_per_keycode;

    // Acknowledge render version and assign atom values
    xcb_render_query_version_reply (_pconn, rendcook, NULL);
    for (int i = 0; i < xa_Count; ++i)
	_atoms[i] = xcb_intern_atom_reply(_pconn, *(xcb_intern_atom_cookie_t*)&_atoms[i], NULL)->atom;

    // Find the root visual
    const xcb_visualtype_t* visual = NULL;
    for (xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(_pscreen); depth_iter.rem; xcb_depth_next(&depth_iter)) {
	if (depth_iter.data->depth != _pscreen->root_depth)
	    continue;
	for (xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(depth_iter.data); visual_iter.rem; xcb_visualtype_next(&visual_iter))
	    if (_pscreen->root_visual == visual_iter.data->visual_id)
		visual = visual_iter.data;
    }
    // Get standard RENDER formats
    xcb_render_query_pict_formats_cookie_t qpfcook = xcb_render_query_pict_formats (_pconn);
    xcb_render_query_pict_formats_reply_t* qpfr = xcb_render_query_pict_formats_reply (_pconn, qpfcook, NULL);
    for (xcb_render_pictforminfo_iterator_t i = xcb_render_query_pict_formats_formats_iterator(qpfr); i.rem; xcb_render_pictforminfo_next(&i)) {
	if (i.data->depth == _pscreen->root_depth && i.data->direct.red_mask == visual->red_mask >> i.data->direct.red_shift)
	    _xrfmt[rfmt_Default] = i.data->id;
	else if (i.data->depth == 1)
	    _xrfmt[rfmt_Bitmask] = i.data->id;
	else if (i.data->depth == 8)
	    _xrfmt[rfmt_Font] = i.data->id;
	else if (i.data->depth == 32 && i.data->direct.red_shift == 16 && i.data->direct.alpha_mask == 0xff)
	    _xrfmt[rfmt_Pixmap] = i.data->id;
    }
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
    for (xcb_generic_event_t* e; !_wantQuit && xcb_flush(_pconn)>0 && (e = xcb_wait_for_event(_pconn)); free(e)) {
	switch (e->response_type & 0x7f) {
	    case XCB_MAP_NOTIFY:	OnMap(); break;
	    case XCB_EXPOSE:		Update(); break;
	    case XCB_CONFIGURE_NOTIFY:	OnResize(e); break;
	    case XCB_KEY_PRESS:		OnKey (TranslateKeycode(e)); break;
	}
    }
    return (EXIT_SUCCESS);
}

void CXApp::Update (void)
{
    if (!_pconn || !_window)
	return;
    OnDraw (GC());
    xcb_render_composite (_pconn, XCB_RENDER_PICT_OP_SRC, _bpict, XCB_NONE, _wpict, 0, 0, 0, 0, 0, 0, _width, _height);
}

//----------------------------------------------------------------------
// Window and mode management
//----------------------------------------------------------------------

void CXApp::CreateWindow (const char* title, int width, int height)
{
    _gc.Resize (width, height);
    // Create the window with given dimensions
    static const uint32_t winvals[] = { XCB_NONE, XCB_EVENT_MASK_EXPOSURE| XCB_EVENT_MASK_KEY_PRESS| XCB_EVENT_MASK_STRUCTURE_NOTIFY };
    xcb_create_window (_pconn, XCB_COPY_FROM_PARENT, _window=xcb_generate_id(_pconn),
	    _pscreen->root, 0, 0, _width = width, _height = height, 0,
	    XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT,
	    XCB_CW_BACK_PIXMAP| XCB_CW_EVENT_MASK, winvals);
    // Create backing pixmap
    xcb_pixmap_t bpixid = xcb_generate_id(_pconn);
    xcb_create_pixmap (_pconn, 32, bpixid, _window, width, height);
    xcb_create_gc (_pconn, _xgc = xcb_generate_id(_pconn), bpixid, 0, NULL);
    xcb_render_create_picture (_pconn, _bpict = xcb_generate_id(_pconn), bpixid, _xrfmt[rfmt_Pixmap], 0, NULL);
    xcb_free_pixmap (_pconn, bpixid);	// henceforth accessed only through _bpict
    xcb_render_create_picture (_pconn, _wpict = xcb_generate_id(_pconn), _window, _xrfmt[rfmt_Default], 0, NULL);
    // Set window title
    xcb_change_property (_pconn, XCB_PROP_MODE_REPLACE, _window, _atoms[xa_WM_NAME], _atoms[xa_STRING], 8, strlen(title), title);
    // Enable WM close message
    xcb_change_property (_pconn, XCB_PROP_MODE_REPLACE, _window, _atoms[xa_WM_PROTOCOLS], _atoms[xa_ATOM], 32, 1, &_atoms[xa_WM_DELETE_WINDOW]);
    // And put it on the screen
    xcb_map_window (_pconn, _window);
}

inline void CXApp::OnMap (void)
{
    // Set the fullscreen flag on the window
    xcb_client_message_event_t xev;
    memset (&xev, 0, sizeof(xev));
    xev.response_type = XCB_CLIENT_MESSAGE;
    xev.window = _window;
    xev.type = _atoms[xa_NET_WM_STATE];
    xev.format = 32;
    xev.data.data32[0] = 1;
    xev.data.data32[1] = _atoms[xa_NET_WM_STATE_FULLSCREEN];
    xcb_send_event (_pconn, _pscreen->root, false, XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT| XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY, (const char*) &xev);
}

inline void CXApp::OnResize (const xcb_generic_event_t* e)
{
    _width = ((const xcb_configure_notify_event_t*)e)->width;
    _height = ((const xcb_configure_notify_event_t*)e)->height;
    // Setup RENDER scaling of the backbuffer
    xcb_render_transform_t tr;
    memset (&tr, 0, sizeof(tr));
    tr.matrix11 = (GC().Width()<<16)/_width;
    tr.matrix22 = (GC().Height()<<16)/_height;
    tr.matrix33 = (1<<16);
    xcb_render_set_picture_transform (_pconn, _bpict, tr);
}

inline wchar_t CXApp::TranslateKeycode (const xcb_generic_event_t* event) const
{
    const xcb_key_press_event_t *kp = (const xcb_key_press_event_t*)event;
    return (_ksyms[(kp->detail-_minKeycode)*_keysymsPerKeycode]);
}

CXApp::SImage CXApp::LoadImage (const char* const* p)
{
    SImage img;
    uint32_t d;
    sscanf (*p++, "%hu %hu %u", &img.w, &img.h, &d);
    uint32_t pal[128];
    for (uint32_t i = 0; i < d; ++i) {
	uint8_t ci; uint32_t cv = 0xff000000;
	sscanf (*p++, "%c c #%X", &ci, &cv);
	pal[ci] = cv ^ 0xff000000;
    }
    vector<uint32_t> pixels (img.w*img.h);
    for (uint32_t y = 0; y < img.h; ++y, ++p)
	for (uint32_t x = 0; x < img.w; ++x)
	    pixels[y*img.w+x] = pal[uint8_t((*p)[x])];

    xcb_pixmap_t pixid = xcb_generate_id(_pconn);
    xcb_create_pixmap (_pconn, 32, pixid, _window, img.w, img.h);
    xcb_put_image (_pconn, XCB_IMAGE_FORMAT_Z_PIXMAP, pixid, _xgc, img.w, img.h, 0, 0, 0, 32, pixels.size()*4, (const uint8_t*)&pixels[0]);
    xcb_render_create_picture (_pconn, img.id = xcb_generate_id(_pconn), pixid, 38, 0, NULL);
    xcb_free_pixmap (_pconn, pixid);
    return (img);
}

void CXApp::DrawImageTile (const SImage& img, const SImageTile& tile, int x, int y)
{
    xcb_render_composite (_pconn, XCB_RENDER_PICT_OP_OVER, img.id, XCB_NONE, _bpict, tile.x, tile.y, 0, 0, x, y, tile.w, tile.h);
}
