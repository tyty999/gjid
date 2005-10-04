// fbdrv.cc
//

#include "fbdrv.h"
#include "cmap.h"
#include "constate.h"

namespace fbgl {

//----------------------------------------------------------------------

/// Default constructor.
CConsoleFramebuffer::CConsoleFramebuffer (void)
: CFramebuffer (),
  m_Fix (),
  m_OrigVar (),
  m_Var (),
  m_Device (),
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
    m_Device.MSync (m_Screen);
    CFramebuffer::SetMode (newMode, depth);
}

/// Sets up the default colormap.
void CConsoleFramebuffer::SetColormap (void)
{
    if (m_Fix.visual == FB_VISUAL_TRUECOLOR)
	return;
    if (!m_Colormap.red) {
	m_Colormap.InitTruecolorValues (m_Var.bits_per_pixel, m_Var.red.length, m_Var.green.length, m_Var.blue.length, m_Fix.visual == FB_VISUAL_DIRECTCOLOR);
	m_Colormap.CopyTo (GC().Palette());
    }
    m_Device.Ioctl (IOCTLID (FBIOPUTCMAP), &m_Colormap);
}

/// Resets the old mode when refocused.
void CConsoleFramebuffer::OnFocus (bool bFocused)
{
    if (bFocused && m_Device.IsOpen()) {
	m_Device.Ioctl (IOCTLID (FBIOPUT_VSCREENINFO), &m_Var);
	m_Device.Ioctl (IOCTLID (FBIOGET_FSCREENINFO), &m_Fix);
	m_Device.Ioctl (IOCTLID (FBIOPAN_DISPLAY), &m_Var);
    }
    CFramebuffer::OnFocus (bFocused);
}

/// Writes the contents of \p gc to screen.
void CConsoleFramebuffer::Flush (void)
{
    if (!m_Device.IsOpen() || !CConsoleState::Instance().IsActive())
	return;

    if (m_Fix.visual == FB_VISUAL_PSEUDOCOLOR) {
	m_Colormap.CopyFrom (GC().Palette());
	m_Device.Ioctl (IOCTLID (FBIOPUTCMAP), &m_Colormap);
    }

    const uint8_t* src = GC().begin();
    uint8_t* dest = (uint8_t*) m_Screen.begin();
    const size_t srclinelen (GC().Size()[0] * m_Var.bits_per_pixel / 8);
    for (uoff_t i = 0; i < GC().Size()[1]; ++i, src+=srclinelen, dest+=m_Fix.line_length)
	copy_n (src, srclinelen, dest);

    m_Device.MSync (m_Screen);
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
    static const long defaultTimeout (200000);
    const utio::CKeyboard& rkb = CConsoleState::Instance().Keyboard();
    rkb.WaitForKeyData (defaultTimeout);
    utio::CKeyboard::metastate_t meta;
    wchar_t key = rkb.GetKey (&meta, false);
    if (key)
	evp->OnKey (TranslateKeycode(key), TranslateKeystate(meta));
}

} // namespace fbgl

