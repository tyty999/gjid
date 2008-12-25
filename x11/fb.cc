// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// fb.cc
//

#include "fb.h"
#include <unistd.h>
#include <errno.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include "mode.h"
#include "xept.h"

namespace fbgl {

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

static int OnXIOError (Display*)
{
    // Xlib will terminate anyway after this call returns, so there is no way to throw.
    CXlibFramebuffer::Instance().OnIOError();
    exit (EXIT_FAILURE);
    return (EXIT_FAILURE);
}

//----------------------------------------------------------------------
// Window and mode management
//----------------------------------------------------------------------

/// Default constructor.
CXlibFramebuffer::CXlibFramebuffer (void)
: CFramebuffer (),
  m_pDisplay (NULL),
  m_pVisual (NULL),
  m_XGC (NULL),
  m_ImageData (),
  m_pImage (NULL),
  m_Window (None)
{
}

/// Virtual destructor. Closes if still open.
CXlibFramebuffer::~CXlibFramebuffer (void)
{
    Close();
}

/// Singleton instance accessor.
/*static*/ CXlibFramebuffer& CXlibFramebuffer::Instance (void)
{
    static CXlibFramebuffer s_Fb;
    return (s_Fb);
}

/// Opens a connection to the X server (no windows opened)
void CXlibFramebuffer::Open (void)
{
    XSetErrorHandler (OnXlibError);
    XSetIOErrorHandler (OnXIOError);

    m_pDisplay = XOpenDisplay (NULL);
    if (!m_pDisplay)
	throw runtime_error ("unable to open an X display, either X is not running or DISPLAY environment variable is not set");
    m_pVisual = DefaultVisual (m_pDisplay, DefaultScreen(m_pDisplay));

    // Save the original video mode.
    XF86VidModeModeInfo omi;
    memset (&omi, 0, sizeof(omi));
    if (!XF86VidModeGetModeLine (m_pDisplay, DefaultScreen(m_pDisplay), (int*) &omi.dotclock, (XF86VidModeModeLine*) &omi.hdisplay))
	throw runtime_error ("unable to determine the current video mode");
    CXlibMode omim;
    omim.ReadFromX (omi);
    SetOrigMode (omim);
    SetCurMode (omim);
    if (omi.c_private)
	XFree (omi.c_private);

    CFramebuffer::Open();
}

/// Closes all active resources, windows, and server connections.
void CXlibFramebuffer::Close (void)
{
    if (!m_pDisplay)
	return;
    CloseWindow();
    if (OrigMode() != CurMode())
	SwitchToMode (OrigMode());
    XSync (m_pDisplay, DefaultScreen(m_pDisplay));
    XCloseDisplay (m_pDisplay);
    m_pDisplay = NULL;
}

/// Loads available modes from the X server.
void CXlibFramebuffer::LoadModes (modevec_t& mv)
{
    int event_base = 0, error_base = 0;
    if (!XF86VidModeQueryExtension (m_pDisplay, &event_base, &error_base))
	throw runtime_error ("this application requires the XF86VidMode server extension");
    int nModes = 0;
    XF86VidModeModeInfo** ppModes = NULL;
    if (!XF86VidModeGetAllModeLines (m_pDisplay, DefaultScreen(m_pDisplay), &nModes, &ppModes) || !ppModes)
	throw runtime_error ("unable to get the video mode list");
    try {
	mv.reserve (nModes);
	CXlibMode m;
	for (int i = 0; i < nModes; ++ i) {
	    m.ReadFromX (*ppModes[i]);
	    mv.push_back (m);
	}
    } catch (...) {
	XFree (ppModes);
	throw;
    }
    XFree (ppModes);
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

/// Puts the main window into full screen mode, which basically means having
/// no decorations, no visible WM parts, etc. X is _extremely_ hostile to
/// this option, so many stupid hacks are required to do it. Obviously, X
/// doesn't want people writing games for it.
///
void CXlibFramebuffer::SetFullscreenMode (bool v)
{
    // ICCCM standard _NET_WM_STATE_FULLSCREEN does not work here because it
    // fixes the window size to screen size, which is larger than the real
    // resolution. Therefore here are some legacy methods to just remove all
    // window decorations. Due to lack of standardization, possibilities abound.
    //
    #define SET_WM_HINTS(hints)	\
	if (v)			\
	    XChangeProperty (m_pDisplay, m_Window, wmh, wmh, BitsInType(int32_t), PropModeReplace, (const uint8_t*) &hints, sizeof(hints)/sizeof(int32_t));	\
	else			\
	    XDeleteProperty (m_pDisplay, m_Window, wmh)
    Atom wmh;
    if (None != (wmh = XInternAtom (m_pDisplay, "_MOTIF_WM_HINTS", True))) {
	struct SMotifWMHints {
	    uint32_t	m_Flags;
	    uint32_t	m_Functions;
	    uint32_t	m_Decorations;
	    int32_t	m_InputMode;
	    uint32_t	m_Status;
	};
	#define MWM_HINT_DECORATIONS	(1 << 1)
	SMotifWMHints Motif_hints = { MWM_HINT_DECORATIONS, 0, 0, 0, 0 };
	SET_WM_HINTS (Motif_hints);
    } else if (None != (wmh = XInternAtom (m_pDisplay, "_WIN_HINTS", True))) {
	int32_t GNOME_hints = 0;
	SET_WM_HINTS (GNOME_hints);
    } else if (None != (wmh = XInternAtom (m_pDisplay, "KWM_WIN_DECORATION", True))) {
	int32_t KDE_hints = 0;
	SET_WM_HINTS (KDE_hints);
    }
    // Legacy methods don't really support fullscreen mode (apparently it's
    // against X Window policy), so removing the decorations does not
    // resize the window, forcing the code to do that manually.
    //
    XWindowChanges wch;
    wch.x = wch.y = 0;
    wch.width = CurMode().Width();
    wch.height = CurMode().Height();
    wch.border_width = 0;
    wch.stack_mode = Above;
    XConfigureWindow (m_pDisplay, m_Window, CWX | CWY | CWWidth | CWHeight | CWBorderWidth | CWStackMode, &wch);
    //
    // Grab the pointer to prevent WM from taking the focus, scrolling the
    // virtual desktop, and otherwise spoiling the game experience.
    //
    if (v)
	XGrabPointer (m_pDisplay, m_Window, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, m_Window, None, CurrentTime);
    else
	XUngrabPointer (m_pDisplay, CurrentTime);
    // Ensure the window stays on top; while the pointer is grabbed there should be no way to circulate.
    XRaiseWindow (m_pDisplay, m_Window);
}

/// Switches to mode \p nm.
void CXlibFramebuffer::SwitchToMode (const CXlibMode& nm)
{
    XF86VidModeModeInfo nmi;
    nm.WriteToX (nmi);
    XF86VidModeSwitchToMode (m_pDisplay, DefaultScreen(m_pDisplay), &nmi);
    XF86VidModeSetViewPort (m_pDisplay, DefaultScreen(m_pDisplay), 0, 0);
    SetCurMode (nm);
}

/// Switches to mode \p m. X does not support depth switching so that is ignored.
void CXlibFramebuffer::SetMode (rcmode_t m, size_t depth)
{
    SwitchToMode (m);
    CloseWindow();	// The old one, if exists.

    // Create the window with full-screen dimensions (not the X screen, the real resolution)
    const int black = BlackPixel (m_pDisplay, DefaultScreen (m_pDisplay));
    m_Window = XCreateSimpleWindow (m_pDisplay, DefaultRootWindow(m_pDisplay), 0, 0, m.Width(), m.Height(), 0, black, black);
    if (m_Window == None)
	throw runtime_error ("unable to create the application window");
    XMapRaised (m_pDisplay, m_Window);

    // Get all relevant events.
    const long eventMask = StructureNotifyMask | ExposureMask | KeyPressMask |
	KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    XSelectInput (m_pDisplay, m_Window, eventMask);

    // Create a gc and a backbuffer image.
    m_XGC = XCreateGC (m_pDisplay, m_Window, 0, NULL);
    const int imageDepth = DefaultDepth (m_pDisplay, DefaultScreen(m_pDisplay));
    m_ImageData.resize (m.Width() * m.Height() * (imageDepth == 24 ? 32 : imageDepth) / 8);
    m_pImage = XCreateImage (m_pDisplay, m_pVisual, imageDepth, ZPixmap, 0, m_ImageData.begin(), m.Width(), m.Height(), 8, 0);
    if (!m_pImage)
	throw bad_alloc (sizeof(XImage));

    // Initialize the palette to grayscale to avoid a black screen if the palette is not set.
    GC().Palette().resize (256);
    for (uoff_t i = 0; i < GC().Palette().size(); ++ i)
	GC().Palette()[i] = RGB (i, i, i);

    CFramebuffer::SetMode (m, depth);
}

//----------------------------------------------------------------------
// Event processing
//----------------------------------------------------------------------

/// Translates Xlib keycode to fbgl equivalent.
static CEventProcessor::key_t TranslateKeycode (int key)
{
    struct SKMap {
	int				xlibValue;
	CEventProcessor::EKeyDataValue	fbglValue;
    };
    static const SKMap kmap[] = {
	{ ' ',		CEventProcessor::key_Space	},
	{ XK_Tab,	CEventProcessor::key_Tab	},
	{ XK_Return,	CEventProcessor::key_Enter	},
	{ XK_Escape,	CEventProcessor::key_Esc	},
	{ XK_BackSpace,	CEventProcessor::key_Backspace	},
	{ XK_KP_5,	CEventProcessor::key_Center	},
	{ XK_Delete,	CEventProcessor::key_Delete	},
	{ XK_Down,	CEventProcessor::key_Down	},
	{ XK_KP_1,	CEventProcessor::key_DownLeft	},
	{ XK_KP_3,	CEventProcessor::key_DownRight	},
	{ XK_End,	CEventProcessor::key_End	},
	{ XK_F1,	CEventProcessor::key_F1		},
	{ XK_F2,	CEventProcessor::key_F2		},
	{ XK_F3,	CEventProcessor::key_F3		},
	{ XK_F4,	CEventProcessor::key_F4		},
	{ XK_F5,	CEventProcessor::key_F5		},
	{ XK_F6,	CEventProcessor::key_F6		},
	{ XK_F7,	CEventProcessor::key_F7		},
	{ XK_F8,	CEventProcessor::key_F8		},
	{ XK_F9,	CEventProcessor::key_F9		},
	{ XK_F10,	CEventProcessor::key_F10	},
	{ XK_F11,	CEventProcessor::key_F11	},
	{ XK_F12,	CEventProcessor::key_F12	},
	{ XK_Home,	CEventProcessor::key_Home	},
	{ XK_Insert,	CEventProcessor::key_Insert	},
	{ XK_Left,	CEventProcessor::key_Left	},
	{ XK_Page_Down,	CEventProcessor::key_PageDown	},
	{ XK_Page_Up,	CEventProcessor::key_PageUp	},
	{ XK_Right,	CEventProcessor::key_Right	},
	{ XK_Up,	CEventProcessor::key_Up		},
	{ XK_KP_7,	CEventProcessor::key_UpLeft	},
	{ XK_KP_9,	CEventProcessor::key_UpRight	}
    };
    for (uoff_t i = 0; i < VectorSize(kmap); ++ i)
	if (kmap[i].xlibValue == key)
	    return (kmap[i].fbglValue);
    return (key);
}

/// Translates xlib key metastate to fbgl equivalents.
static wchar_t TranslateKeystate (int kbms)
{
    static const int metamap[] = { ShiftMask, Mod1Mask, ControlMask };
    wchar_t ks;
    for (uoff_t i = 0; i < VectorSize(metamap); ++ i)
	if (kbms & metamap[i])
	    ks |= 1 << (i + 24);
    return (ks);
}

/// Decodes and executes a button event.
inline void CXlibFramebuffer::DecodeButton (CEventProcessor* pep, const XButtonEvent& e)
{
    pep->OnButtonDown (e.button, e.x, e.y);
}

/// Decodes and executes a motion event.
inline void CXlibFramebuffer::DecodeMotion (CEventProcessor* pep, const XMotionEvent& e)
{
    pep->OnMouseMove (e.x, e.y);
}

/// Decodes and executes a key event.
inline void CXlibFramebuffer::DecodeKey (CEventProcessor* pep, const XKeyEvent& e)
{
    if (e.type == KeyRelease)
	return;
    const int ksym = XKeycodeToKeysym (m_pDisplay, e.keycode, 0);
    pep->OnKey (TranslateKeycode (ksym) | TranslateKeystate (e.state));
}

void CXlibFramebuffer::CheckEvents (CEventProcessor* pep)
{
    while (XPending (m_pDisplay)) {
	if (g_bErrorHappened)
	    throw XlibError (g_ErrorEvent);
	XEvent e;
	XNextEvent (m_pDisplay, &e);
	switch (e.type) {
	    case MapNotify:	SetFullscreenMode();
	    case Expose:	Flush();			break;
	    case ButtonPress:
	    case ButtonRelease:	DecodeButton (pep, e.xbutton);	break;
	    case MotionNotify:	DecodeMotion (pep, e.xmotion);	break;
	    case KeyPress:
	    case KeyRelease:	DecodeKey (pep, e.xkey);	break;
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

/// Called when X server connection is abruptly terminated.
void CXlibFramebuffer::OnIOError (void)
{
    cout.flush();
    cerr << "Error: the connection to the X server has been unexpectedly terminated" << endl;
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

} // namespace fbgl

