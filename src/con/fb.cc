// fbdrv.cc
//

#include "fbdrv.h"
#include "cmap.h"
#include "constate.h"

namespace fbgl {

//----------------------------------------------------------------------

const CFbMode CConsoleFramebuffer::s_NullMode;

//----------------------------------------------------------------------

/// Default constructor.
CConsoleFramebuffer::CConsoleFramebuffer (void)
: CFramebuffer (),
  m_Fix (),
  m_OrigVar (),
  m_Var (),
  m_Device (),
  m_Modes (),
  m_Colormap ()
{
    CConsoleState::Instance().RegisterFramebuffer (this);
}

/// Virtual destructor.
CConsoleFramebuffer::~CConsoleFramebuffer (void)
{
    CConsoleState::Instance().RegisterFramebuffer (NULL);
    Close();
}

/// Singleton interface.
/*static*/ CConsoleFramebuffer& CConsoleFramebuffer::Instance (void)
{
    static CConsoleFramebuffer s_fb;
    return (s_fb);
}

/// Opens the framebuffer device and enables graphics mode.
void CConsoleFramebuffer::Open (void)
{
    assert (!m_Device.IsOpen());
    string deviceName;
    DetectDefaultDevice (deviceName);
    m_Device.Open (deviceName, CFile::mode_Modify);
    m_Device.Ioctl (IOCTLID (FBIOGET_FSCREENINFO), &m_Fix);
    m_Device.Ioctl (IOCTLID (FBIOGET_VSCREENINFO), &m_OrigVar);
    m_Var = m_OrigVar;
    m_Screen = m_Device.Map (m_Fix.smem_len);
    LoadModes();
    CConsoleState::Instance().EnterGraphicsMode();
}

/// Leaves graphics mode and closes the device.
void CConsoleFramebuffer::Close (void)
{
    CConsoleState::Instance().LeaveGraphicsMode();
    if (m_Screen.data())
	m_Device.Unmap (m_Screen);
    if (m_Device.IsOpen()) {
	m_Device.Ioctl (IOCTLID (FBIOPUT_VSCREENINFO), &m_OrigVar);
	m_Device.Close();
    }
}

/// Detects the default framebuffer device for use with the attached tty.
///
/// This works by trying to open fb0 and getting the console-framebuffer
/// mapping for the standard input device.
///
void CConsoleFramebuffer::DetectDefaultDevice (string& deviceName) const
{
    const int vti = CConsoleState::Instance().VtIndex();
    if (vti < 0)
	throw domain_error ("this program only works on a real console, not in an xterm or ssh");
    CFile fb;
    fb.Open ("/dev/fb0", CFile::mode_Write);
    struct fb_con2fbmap c2fb = { vti, 0 };
    fb.Ioctl (IOCTLID (FBIOGET_CON2FBMAP), &c2fb);
    deviceName.format ("/dev/fb%d", c2fb.framebuffer);
}

/// Loads available video modes from /etc/fb.modes
void CConsoleFramebuffer::LoadModes (void)
{
    string mdbt, reader;
    mdbt.read_file ("/etc/fb.modes");
    foreach (string::const_iterator, i, mdbt) {
	m_Modes.push_back();
	reader.link (i, mdbt.end());
	i = m_Modes.back().ReadFromModedb (reader);
    }
    if (!m_Modes.empty() && m_Modes.back().Name().empty())
	m_Modes.pop_back();
}

/// Looks up a video mode closest to the given parameters.
const CFbMode& CConsoleFramebuffer::FindClosestMode (size_t w, size_t h, size_t freq) const
{
    uoff_t found (m_Modes.size());
    size_t diff (SIZE_MAX);
    foreach (modevec_t::const_iterator, m, m_Modes) {
	const size_t md = absv<int>(m->Width() - w) +
			  absv<int>(m->Height() - h) +
			  absv<int>(m->RefreshRate() - freq);
	if (md < diff) {
	    found = distance (m_Modes.begin(), m);
	    diff = md;
	}
    }
    return (found < m_Modes.size() ? m_Modes[found] : s_NullMode);
}

/// Called when the vt gains or loses focus.
void CConsoleFramebuffer::OnFocus (bool)
{
}

/// Changes to another mode.
void CConsoleFramebuffer::SetMode (CFbMode newMode, size_t depth)
{
    assert (m_Device.IsOpen());
    newMode.SetDepth (depth);
    newMode.CreateVarinfo (m_Var);
    m_Device.Ioctl (IOCTLID (FBIOPUT_VSCREENINFO), &m_Var);
    m_Device.Ioctl (IOCTLID (FBIOGET_FSCREENINFO), &m_Fix);
    m_Device.Ioctl (IOCTLID (FBIOPAN_DISPLAY), &m_Var);
    SetColormap();
    for (uoff_t i = 0; i < m_Screen.size(); ++ i)
	*(m_Screen.begin() + i) = i % 128;
    m_Device.MSync (m_Screen);
}

/// Sets up the default colormap.
void CConsoleFramebuffer::SetColormap (void)
{
    if (m_Fix.visual == FB_VISUAL_TRUECOLOR)
	return;
    if (!m_Colormap.red)
	m_Colormap.InitTruecolorValues (m_Var.bits_per_pixel, m_Var.red.length, m_Var.green.length, m_Var.blue.length, m_Fix.visual == FB_VISUAL_DIRECTCOLOR);
    m_Device.Ioctl (IOCTLID (FBIOPUTCMAP), &m_Colormap);
}

/// Returns pointer to the screen.
memlink CConsoleFramebuffer::Pixels (void)
{
    return (m_Screen);
}

/// Returns the size of the screen.
Size2d CConsoleFramebuffer::Size (void)
{
    return (Size2d (m_Var.xres, m_Var.yres));
}

/// Writes the contents of \p gc to screen.
void CConsoleFramebuffer::Flush (const CGC& gc)
{
    if (m_Fix.visual == FB_VISUAL_PSEUDOCOLOR) {
	m_Colormap.CopyFrom (gc.Palette());
	m_Device.Ioctl (IOCTLID (FBIOPUTCMAP), &m_Colormap);
    }
}

/// Translates utio key codes to fbgl equivalents.
static key_t TranslateKeycode (wchar_t key)
{
    struct SKMap {
	utio::EKeyDataValue	utioValue;
	EKeyDataValue		fbglValue;
    };
    static const SKMap kmap[] = {
	{ utio::kv_Space,	key_Space	},
	{ utio::kv_Tab,		key_Tab		},
	{ utio::kv_Enter,	key_Enter	},
	{ utio::kv_Esc,		key_Esc		},
	{ utio::kv_Backspace,	key_Backspace	},
	{ utio::kv_Center,	key_Center	},
	{ utio::kv_Close,	key_Close	},
	{ utio::kv_Delete,	key_Delete	},
	{ utio::kv_Down,	key_Down	},
	{ utio::kv_DownLeft,	key_DownLeft	},
	{ utio::kv_DownRight,	key_DownRight	},
	{ utio::kv_End,		key_End		},
	{ utio::kv_F0,		key_F0		},
	{ utio::kv_F1,		key_F1		},
	{ utio::kv_F2,		key_F2		},
	{ utio::kv_F3,		key_F3		},
	{ utio::kv_F4,		key_F4		},
	{ utio::kv_F5,		key_F5		},
	{ utio::kv_F6,		key_F6		},
	{ utio::kv_F7,		key_F7		},
	{ utio::kv_F8,		key_F8		},
	{ utio::kv_F9,		key_F9		},
	{ utio::kv_F10,		key_F10		},
	{ utio::kv_F11,		key_F11		},
	{ utio::kv_F12,		key_F12		},
	{ utio::kv_Home,	key_Home	},
	{ utio::kv_Insert,	key_Insert	},
	{ utio::kv_Left,	key_Left	},
	{ utio::kv_PageDown,	key_PageDown	},
	{ utio::kv_PageUp,	key_PageUp	},
	{ utio::kv_Right,	key_Right	},
	{ utio::kv_Up,		key_Up		},
	{ utio::kv_UpLeft,	key_UpLeft	},
	{ utio::kv_UpRight,	key_UpRight	}
    };
    for (uoff_t i = 0; i < VectorSize(kmap); ++ i)
	if (kmap[i].utioValue == key)
	    return (kmap[i].fbglValue);
    return (key);
};

/// Translates utio key metastate to fbgl equivalents.
static keystate_t TranslateKeystate (utio::CKeyboard::metastate_t kbms)
{
    static const uint8_t metamap[] = {
	ks_Shift,
	ks_Alt,
	ks_Ctrl
    };
    keystate_t ks;
    for (uoff_t i = 0; i < VectorSize(metamap); ++ i)
	if (kbms[i])
	    ks.set (metamap[i]);
    return (ks);
}

/// Waits for and reads any UI events.
void CConsoleFramebuffer::CheckEvents (CEventProcessor* evp) const
{
    static const long defaultTimeout (200);
    const utio::CKeyboard& rkb = CConsoleState::Instance().Keyboard();
    rkb.WaitForKeyData (defaultTimeout);
    utio::CKeyboard::metastate_t meta;
    wchar_t key = rkb.GetKey (&meta, false);
    if (key)
	evp->OnKey (TranslateKeycode(key), TranslateKeystate(meta));
}

} // namespace fbgl

