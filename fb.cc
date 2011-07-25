// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "fb.h"
#include "app.h"
#include <unistd.h>
#include <errno.h>
#include <X11/Xutil.h>

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
: m_GC()
, m_pDisplay (NULL)
, m_pVisual (NULL)
, m_XGC (NULL)
, m_ImageData()
, m_pImage (NULL)
, m_Window (None)
{
}

/// Opens a connection to the X server (no windows opened)
void CXlibFramebuffer::Open (void)
{
    XSetErrorHandler (OnXlibError);

    m_pDisplay = XOpenDisplay (NULL);
    if (!m_pDisplay)
	throw runtime_error ("unable to open an X display, either X is not running or DISPLAY environment variable is not set");
    m_pVisual = DefaultVisual (m_pDisplay, DefaultScreen(m_pDisplay));
}

/// Closes all active resources, windows, and server connections.
void CXlibFramebuffer::Close (void)
{
    if (!m_pDisplay)
	return;
    CloseWindow();
    XSync (m_pDisplay, DefaultScreen(m_pDisplay));
    XCloseDisplay (m_pDisplay);
    m_pDisplay = NULL;
}

/// Closes the application window and frees its resources.
void CXlibFramebuffer::CloseWindow (void)
{
    if (!m_pDisplay || m_Window == None)
	return;
    if (m_XGC) {
	XFreeGC (m_pDisplay, m_XGC);
	m_XGC = NULL;
    }
    XUngrabPointer (m_pDisplay, CurrentTime);
    if (m_pImage) {
	m_pImage->data = NULL;	// Managed by m_ImageData
	XDestroyImage (m_pImage);
	m_pImage = NULL;
    }
    XUnmapWindow (m_pDisplay, m_Window);
    XDestroyWindow (m_pDisplay, m_Window);
    m_Window = None;
}

void CXlibFramebuffer::SetStandardMode (EStdFbMode, size_t)
{
    CloseWindow();	// The old one, if exists.

    // Create the window with full-screen dimensions
    const int black = BlackPixel (m_pDisplay, DefaultScreen (m_pDisplay));
    const dim_t width = DisplayWidth (m_pDisplay, DefaultScreen (m_pDisplay));
    const dim_t height = DisplayHeight (m_pDisplay, DefaultScreen (m_pDisplay));
    m_Window = XCreateSimpleWindow (m_pDisplay, DefaultRootWindow(m_pDisplay), 0, 0, width, height, 0, black, black);
    if (m_Window == None)
	throw runtime_error ("unable to create the application window");

    // Set the fullscreen flag on the window
    XEvent xev;
    xev.type = ClientMessage;
    xev.xclient.window = m_Window;
    xev.xclient.message_type = XInternAtom (m_pDisplay, "_NET_WM_STATE", True);
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = True;
    xev.xclient.data.l[1] = XInternAtom (m_pDisplay, "_NET_WM_STATE_FULLSCREEN", True);
    XSendEvent (m_pDisplay, m_Window, False, SubstructureNotifyMask, &xev);

    // Get all relevant events.
    XMapRaised (m_pDisplay, m_Window);
    const long eventMask = StructureNotifyMask | ExposureMask | KeyPressMask |
	KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    XSelectInput (m_pDisplay, m_Window, eventMask);

    // Create a gc and a backbuffer image.
    m_XGC = XCreateGC (m_pDisplay, m_Window, 0, NULL);
    const int imageDepth = DefaultDepth (m_pDisplay, DefaultScreen(m_pDisplay));
    m_ImageData.resize (width * height * (imageDepth == 24 ? 32 : imageDepth) / 8);
    m_pImage = XCreateImage (m_pDisplay, m_pVisual, imageDepth, ZPixmap, 0, m_ImageData.begin(), width, height, 8, 0);
    if (!m_pImage)
	throw bad_alloc (sizeof(XImage));

    // Initialize the palette to grayscale to avoid a black screen if the palette is not set.
    GC().Palette().resize (256);
    for (uoff_t i = 0; i < GC().Palette().size(); ++ i)
	GC().Palette()[i] = RGB (i, i, i);
    m_GC.Resize (320, 240);
}

//----------------------------------------------------------------------
// Event processing
//----------------------------------------------------------------------

void CXlibFramebuffer::CheckEvents (CApp* papp)
{
    while (XPending (m_pDisplay)) {
	if (g_bErrorHappened)
	    throw XlibError (g_ErrorEvent);
	XEvent e;
	XNextEvent (m_pDisplay, &e);
	key_t keymods = e.xkey.state;
	switch (e.type) {
	    case MapNotify:	break;
	    case Expose:	Flush(); break;
	    case ButtonRelease:	papp->OnButtonDown (e.xbutton.button, e.xbutton.x, e.xbutton.y); break;
	    case MotionNotify:	papp->OnMouseMove (e.xmotion.x, e.xmotion.y); break;
	    case KeyRelease:	keymods |= ModKeyReleasedMask;
	    case KeyPress:	papp->OnKey (XKeycodeToKeysym(m_pDisplay, e.xkey.keycode, 0)|
					    ((keymods << _XKM_Bitshift) & XKM_Mask)); break;
	}
    }
    WaitForEvents();
}

/// Waits for X events with timeout.
void CXlibFramebuffer::WaitForEvents (void)
{
    fd_set fds;
    FD_ZERO (&fds); 
    FD_SET (ConnectionNumber (m_pDisplay), &fds);
    struct timeval tv = { 0, 200000 };
    int rv;
    do {
	errno = 0;
	rv = select (ConnectionNumber(m_pDisplay) + 1, &fds, NULL, NULL, &tv);
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
    PixelType* dest = (PixelType*) m_pImage->data;

    // Scale the gc to the screen resolution.
    const size_t sw = GC().Width(), sh = GC().Height();
    const size_t dw = m_pImage->width, dh = m_pImage->height;
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
    if (!m_pDisplay || m_Window == None || !m_pImage)
	return;
    if (m_pImage->depth == 16)
	CopyGCToImage<uint16_t>();
    else if (m_pImage->depth == 24 || m_pImage->depth == 32)
	CopyGCToImage<uint32_t>();
    XPutImage (m_pDisplay, m_Window, m_XGC, m_pImage, 0, 0, 0, 0, m_pImage->width, m_pImage->height);
}
