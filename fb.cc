// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "fb.h"
#include "app.h"
#include <unistd.h>
#include <errno.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

//----------------------------------------------------------------------

class XlibError : public runtime_error {
public:
			XlibError (const XErrorEvent& e) throw();
    virtual const char*	what (void) const throw() { return ("Xlib error"); }
};

XlibError::XlibError (const XErrorEvent& e) throw()
: runtime_error ("")
{
    char errbuf [512];
    XGetErrorText (e.display, e.error_code, VectorBlock(errbuf));
    try { m_Arg.format ("%hhu,%hhu: %s", e.request_code, e.minor_code, errbuf); } catch (...) {}
}

//----------------------------------------------------------------------
// Can't throw an exception through a C callstack, hence this junk.
//
static XErrorEvent g_ErrorEvent;
static bool g_bErrorHappened = false;

static int OnXlibError (Display*, XErrorEvent* e)
{
    if (!g_bErrorHappened && e) {
	g_bErrorHappened = true;
	g_ErrorEvent = *e;
    }
    return (EXIT_SUCCESS);
}

//----------------------------------------------------------------------
// Window and mode management
//----------------------------------------------------------------------

/// Default constructor.
CXlibFramebuffer::CXlibFramebuffer (void)
:_gc()
,_pDisplay (NULL)
,_pVisual (NULL)
,_xgc (NULL)
,_imageData()
,_pImage (NULL)
,_window (None)
{
}

/// Opens a connection to the X server (no windows opened)
void CXlibFramebuffer::Open (void)
{
    XSetErrorHandler (OnXlibError);

    _pDisplay = XOpenDisplay (NULL);
    if (!_pDisplay)
	throw runtime_error ("unable to open an X display, either X is not running or DISPLAY environment variable is not set");
    _pVisual = DefaultVisual (_pDisplay, DefaultScreen(_pDisplay));
}

/// Closes all active resources, windows, and server connections.
void CXlibFramebuffer::Close (void)
{
    if (!_pDisplay)
	return;
    CloseWindow();
    XSync (_pDisplay, DefaultScreen(_pDisplay));
    XCloseDisplay (_pDisplay);
    _pDisplay = NULL;
}

/// Closes the application window and frees its resources.
void CXlibFramebuffer::CloseWindow (void)
{
    if (!_pDisplay || _window == None)
	return;
    if (_xgc) {
	XFreeGC (_pDisplay, _xgc);
	_xgc = NULL;
    }
    XUngrabPointer (_pDisplay, CurrentTime);
    if (_pImage) {
	_pImage->data = NULL;	// Managed by _imageData
	XDestroyImage (_pImage);
	_pImage = NULL;
    }
    XUnmapWindow (_pDisplay, _window);
    XDestroyWindow (_pDisplay, _window);
    _window = None;
}

void CXlibFramebuffer::CreateWindow (const char* title, coord_t width, coord_t height)
{
    CloseWindow();	// The old one, if exists.

    // Create the window with full-screen dimensions
    const int black = BlackPixel (_pDisplay, DefaultScreen (_pDisplay));
    _window = XCreateSimpleWindow (_pDisplay, DefaultRootWindow(_pDisplay), 0, 0, width, height, 0, black, black);
    if (_window == None)
	throw runtime_error ("unable to create the application window");
    // Create a GC for this window
    _xgc = XCreateGC (_pDisplay, _window, 0, NULL);
    // Enable WM close message
    Atom xa_w_protocols = XInternAtom (_pDisplay, "WM_PROTOCOLS", False);
    Atom xa_w_delete_window = XInternAtom (_pDisplay, "WM_DELETE_WINDOW", False);
    XChangeProperty (_pDisplay, _window, xa_w_protocols, XA_ATOM, 32, PropModeReplace, (const unsigned char*) &xa_w_delete_window, 1);
    // Set window title
    XChangeProperty (_pDisplay, _window, XA_WM_NAME, XA_STRING, 8, PropModeReplace, (const unsigned char*) title, strlen(title));
    // Get all relevant events.
    const long eventMask = StructureNotifyMask | ExposureMask | KeyPressMask |
	KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    XSelectInput (_pDisplay, _window, eventMask);
    // And put it on the screen
    XMapRaised (_pDisplay, _window);

    // Initialize the palette to grayscale to avoid a black screen if the palette is not set.
    GC().Palette().resize (256);
    for (uoff_t i = 0; i < GC().Palette().size(); ++ i)
	GC().Palette()[i] = RGB (i, i, i);
    _gc.Resize (width, height);
}

inline void CXlibFramebuffer::OnMap (void)
{
    // Set the fullscreen flag on the window
    XEvent xev;
    memset (&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.display = _pDisplay;
    xev.xclient.window = _window;
    xev.xclient.message_type = XInternAtom (_pDisplay, "_NET_WM_STATE", False);
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;
    xev.xclient.data.l[1] = XInternAtom (_pDisplay, "_NET_WM_STATE_FULLSCREEN", False);
    XSendEvent (_pDisplay, DefaultRootWindow(_pDisplay), False, SubstructureRedirectMask|SubstructureNotifyMask, &xev);
}

inline void CXlibFramebuffer::OnConfigure (coord_t width, coord_t height)
{
    const int imageDepth = DefaultDepth (_pDisplay, DefaultScreen(_pDisplay));
    _imageData.resize (width * height * (imageDepth == 24 ? 32 : imageDepth) / 8);
    // Create the backbuffer image.
    if (_pImage) {
	_pImage->data = NULL;	// Managed by _imageData
	XDestroyImage (_pImage);
    }
    _pImage = XCreateImage (_pDisplay, _pVisual, imageDepth, ZPixmap, 0, _imageData.begin(), width, height, 8, 0);
    if (!_pImage)
	throw bad_alloc (sizeof(XImage));
}

//----------------------------------------------------------------------
// Event processing
//----------------------------------------------------------------------

void CXlibFramebuffer::CheckEvents (CApp* papp)
{
    while (XPending (_pDisplay)) {
	if (g_bErrorHappened)
	    throw XlibError (g_ErrorEvent);
	XEvent e;
	XNextEvent (_pDisplay, &e);
	key_t keymods = e.xkey.state;
	switch (e.type) {
	    case MapNotify:	OnMap(); break;
	    case ConfigureNotify: OnConfigure (e.xconfigure.width, e.xconfigure.height); break;
	    case Expose:	Flush(); break;
	    case ButtonPress:	papp->OnButton (e.xbutton.button, e.xbutton.x, e.xbutton.y); break;
	    case ButtonRelease:	papp->OnButtonUp (e.xbutton.button, e.xbutton.x, e.xbutton.y); break;
	    case MotionNotify:	papp->OnMouseMove (e.xmotion.x, e.xmotion.y); break;
	    case KeyPress:	papp->OnKey (XKeycodeToKeysym(_pDisplay, e.xkey.keycode, 0)|((keymods << _XKM_Bitshift) & XKM_Mask)); break;
	    case KeyRelease:	papp->OnKeyUp (XKeycodeToKeysym(_pDisplay, e.xkey.keycode, 0)|((keymods << _XKM_Bitshift) & XKM_Mask)); break;
	}
    }
    WaitForEvents();
}

/// Waits for X events with timeout.
void CXlibFramebuffer::WaitForEvents (void)
{
    fd_set fds;
    FD_ZERO (&fds); 
    FD_SET (ConnectionNumber (_pDisplay), &fds);
    struct timeval tv = { 0, 200000 };
    int rv;
    do {
	errno = 0;
	rv = select (ConnectionNumber(_pDisplay) + 1, &fds, NULL, NULL, &tv);
    } while (errno == EINTR); 
    if (rv < 0)
	throw file_exception ("select", "X server connection");
}

//----------------------------------------------------------------------
// Drawing and colormap translation.
//----------------------------------------------------------------------

/// Creates a list of truecolor values from GC palette entries.
template <typename PixelType>
void CXlibFramebuffer::InitColormap (PixelType* cmap) const
{
    const CPalette& rpal (GC().Palette());
    for (uoff_t i = 0; i < min (256U, rpal.size()); ++ i) {
	if (BitsInType (PixelType) == 32)
	    cmap[i] = rpal[i];
	else if (BitsInType (PixelType) == 16) {
	    ray_t r, g, b;
	    unRGB (rpal[i], r, g, b);
	    cmap[i] = (r >> 3) << 11 | (g >> 2) << 5 | b >> 3;
	}
    }
}

/// Copies GC contents to the image.
template <typename PixelType>
void CXlibFramebuffer::CopyGCToImage (void)
{
    PixelType cmap [256];
    InitColormap (cmap);
    const color_t* src = GC().begin();
    PixelType* dest = (PixelType*) _pImage->data;

    // Scale the gc to the screen resolution.
    const size_t sw = GC().Width(), sh = GC().Height();
    const size_t dw = _pImage->width, dh = _pImage->height;
    size_t dx = 0, dy = 0;
    for (size_t y = 0; y < sh; ++y) {
	for (; dy < dh; dy += sh) {
	    for (size_t x = 0; x < sw; ++x) {
		const PixelType v (cmap[src[x]]);
		for (; dx < dw; dx += sw)
		    *dest++ = v;
		dx -= dw;
	    }
	}
	dy -= dh;
	src += sw;
    }
}

/// Copies the GC to an XImage and flushes the image to the server.
void CXlibFramebuffer::Flush (void)
{
    if (!_pDisplay || _window == None || !_pImage)
	return;
    if (_pImage->depth == 16)
	CopyGCToImage<uint16_t>();
    else if (_pImage->depth == 24 || _pImage->depth == 32)
	CopyGCToImage<uint32_t>();
    XPutImage (_pDisplay, _window, _xgc, _pImage, 0, 0, 0, 0, _pImage->width, _pImage->height);
}
