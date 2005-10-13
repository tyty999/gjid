// xfb.cc
//

#include "xfb.h"
#include <unistd.h>
#include <errno.h>
#include <X11/keysym.h>

namespace fbgl {

CXlibFramebuffer::CXlibFramebuffer (void)
: CFramebuffer (),
  m_pDisplay (NULL),
  m_pVisual (NULL),
  m_XGC (NULL),
  m_Window ()
{
}

CXlibFramebuffer::~CXlibFramebuffer (void)
{
    Close();
}

/*static*/ CXlibFramebuffer& CXlibFramebuffer::Instance (void)
{
    static CXlibFramebuffer s_Fb;
    return (s_Fb);
}

void CXlibFramebuffer::Open (void)
{
    m_pDisplay = XOpenDisplay (NULL);
    if (!m_pDisplay)
	throw runtime_error ("unable to open an X display, either X is not running or DISPLAY environment variable is not set");

    int blackColor = BlackPixel (m_pDisplay, DefaultScreen (m_pDisplay));
    m_Window = XCreateSimpleWindow (m_pDisplay, DefaultRootWindow(m_pDisplay), 0, 0, 640, 480, 0, blackColor, blackColor);

    XSelectInput (m_pDisplay, m_Window, StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask);
    XMapWindow (m_pDisplay, m_Window);

    m_XGC = XCreateGC (m_pDisplay, m_Window, 0, NULL);
    XFlush (m_pDisplay);
}

void CXlibFramebuffer::Close (void)
{
    if (!m_pDisplay)
	return;
    XUnmapWindow (m_pDisplay, m_Window);
    XCloseDisplay (m_pDisplay);
    m_pDisplay = NULL;
}

void CXlibFramebuffer::SetMode (CFbMode m, size_t depth)
{
    CFramebuffer::SetMode (m, depth);
}

void CXlibFramebuffer::CheckEvents (CEventProcessor* pep)
{
    while (XPending (m_pDisplay)) {
	XEvent e;
	XNextEvent (m_pDisplay, &e);
	switch (e.type) {
	    case MapNotify:
	    case Expose:	Flush();			break;
	    case ButtonPress:	DecodeButton (pep, e.xbutton);	break;
	    case MotionNotify:	DecodeMotion (pep, e.xmotion);	break;
	    case KeyPress:
	    case KeyRelease:	DecodeKey (pep, e.xkey);	break;
	}
    }
    WaitForEvents();
}

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
static CEventProcessor::keystate_t TranslateKeystate (int kbms)
{
    static const int metamap[] = {
	Mod1Mask,
	ControlMask,
	ShiftMask
    };
    CEventProcessor::keystate_t ks;
    for (uoff_t i = 0; i < VectorSize(metamap); ++ i)
	if (kbms & metamap[i])
	    ks.set (i);
    return (ks);
}

void CXlibFramebuffer::DecodeButton (CEventProcessor* pep, const XButtonEvent& e)
{
    pep->OnButtonDown (e.button, e.x, e.y, TranslateKeystate (e.state));
}

void CXlibFramebuffer::DecodeMotion (CEventProcessor* pep, const XMotionEvent& e)
{
    pep->OnMouseMove (e.x, e.y, TranslateKeystate (e.state));
}

void CXlibFramebuffer::DecodeKey (CEventProcessor* pep, const XKeyEvent& e)
{
    if (e.type == KeyRelease)
	return;
    const int ksym = XKeycodeToKeysym (m_pDisplay, e.keycode, 0);
    pep->OnKey (TranslateKeycode (ksym), TranslateKeystate (e.state));
}

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

void CXlibFramebuffer::Flush (void)
{
    XSetForeground (m_pDisplay, m_XGC, WhitePixel (m_pDisplay, DefaultScreen (m_pDisplay)));
    XDrawLine (m_pDisplay, m_Window, m_XGC, 10, 60, 180, 20);
    XFlush (m_pDisplay);
}

} // namespace fbgl

