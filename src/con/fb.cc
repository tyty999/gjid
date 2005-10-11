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
    if (m_Screen.data())
	m_Device.Unmap (m_Screen);
    if (m_Device.IsOpen()) {
	m_Var = m_OrigVar;
	SyncScreeninfo();
	m_Device.Close();
    }
    CConsoleState::Instance().LeaveGraphicsMode();
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

/// Writes the varinfo to the fb and reads the parameters back.
void CConsoleFramebuffer::SyncScreeninfo (void)
{
    m_Device.Ioctl (IOCTLID (FBIOPUT_VSCREENINFO), &m_Var);
    m_Device.Ioctl (IOCTLID (FBIOGET_FSCREENINFO), &m_Fix);
    m_Device.Ioctl (IOCTLID (FBIOPAN_DISPLAY), &m_Var);
}

/// Changes to another mode.
void CConsoleFramebuffer::SetMode (CFbMode newMode, size_t depth)
{
    assert (m_Device.IsOpen());
    newMode.SetDepth (depth);
    newMode.CreateVarinfo (m_Var);
    SyncScreeninfo();
    SetColormap();
    m_Device.MSync (m_Screen);
    CFramebuffer::SetMode (newMode, depth);
}

/// Sets a standard mode.
void CConsoleFramebuffer::SetStandardMode (EStdFbMode m, size_t freq)
{
    if (m == stdmode_Text) {
	m_Var = m_OrigVar;
	SyncScreeninfo();
	CConsoleState::Instance().LeaveGraphicsMode();
    }
    CFramebuffer::SetStandardMode (m, freq);
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
    if (bFocused && m_Device.IsOpen())
	SyncScreeninfo();
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
    const Size2d gcsz (GC().Size());
    const size_t srclinelen (gcsz[0] * m_Var.bits_per_pixel / 8);

    if (gcsz[0] == m_Var.xres) {
	for (uoff_t i = 0; i < gcsz[1]; ++i, src+=srclinelen, dest+=m_Fix.line_length)
	    copy_n (src, srclinelen, dest);
    } else if (gcsz[0] == 320 && m_Var.xres == 640) { // Special case for 320x240 mode, emulated by doubling.
	const uint8_t* ls (src);
	for (uoff_t y = 0; y < 480; ++y, dest+=m_Fix.line_length-640) {
	    for (const uint8_t* s = ls; s < ls + 320; s += 8, dest += 16) {
		for (uoff_t i = 0; i < 8; ++ i)
		    dest[i * 2] = s[i];
	    }
	    if (y % 2)
		ls += 320;
	}
    }

    m_Device.MSync (m_Screen);
}

/// Translates utio key codes to fbgl equivalents.
static key_t TranslateKeycode (wchar_t key)
{
    struct SKMap {
	utio::EKeyDataValue		utioValue;
	CEventProcessor::EKeyDataValue	fbglValue;
    };
    static const SKMap kmap[] = {
	{ utio::kv_Space,	CEventProcessor::key_Space	},
	{ utio::kv_Tab,		CEventProcessor::key_Tab	},
	{ utio::kv_Enter,	CEventProcessor::key_Enter	},
	{ utio::kv_Esc,		CEventProcessor::key_Esc	},
	{ utio::kv_Backspace,	CEventProcessor::key_Backspace	},
	{ utio::kv_Center,	CEventProcessor::key_Center	},
	{ utio::kv_Close,	CEventProcessor::key_Close	},
	{ utio::kv_Delete,	CEventProcessor::key_Delete	},
	{ utio::kv_Down,	CEventProcessor::key_Down	},
	{ utio::kv_DownLeft,	CEventProcessor::key_DownLeft	},
	{ utio::kv_DownRight,	CEventProcessor::key_DownRight	},
	{ utio::kv_End,		CEventProcessor::key_End	},
	{ utio::kv_F0,		CEventProcessor::key_F0		},
	{ utio::kv_F1,		CEventProcessor::key_F1		},
	{ utio::kv_F2,		CEventProcessor::key_F2		},
	{ utio::kv_F3,		CEventProcessor::key_F3		},
	{ utio::kv_F4,		CEventProcessor::key_F4		},
	{ utio::kv_F5,		CEventProcessor::key_F5		},
	{ utio::kv_F6,		CEventProcessor::key_F6		},
	{ utio::kv_F7,		CEventProcessor::key_F7		},
	{ utio::kv_F8,		CEventProcessor::key_F8		},
	{ utio::kv_F9,		CEventProcessor::key_F9		},
	{ utio::kv_F10,		CEventProcessor::key_F10	},
	{ utio::kv_F11,		CEventProcessor::key_F11	},
	{ utio::kv_F12,		CEventProcessor::key_F12	},
	{ utio::kv_Home,	CEventProcessor::key_Home	},
	{ utio::kv_Insert,	CEventProcessor::key_Insert	},
	{ utio::kv_Left,	CEventProcessor::key_Left	},
	{ utio::kv_PageDown,	CEventProcessor::key_PageDown	},
	{ utio::kv_PageUp,	CEventProcessor::key_PageUp	},
	{ utio::kv_Right,	CEventProcessor::key_Right	},
	{ utio::kv_Up,		CEventProcessor::key_Up		},
	{ utio::kv_UpLeft,	CEventProcessor::key_UpLeft	},
	{ utio::kv_UpRight,	CEventProcessor::key_UpRight	}
    };
    for (uoff_t i = 0; i < VectorSize(kmap); ++ i)
	if (kmap[i].utioValue == key)
	    return (kmap[i].fbglValue);
    return (key);
};

/// Translates utio key metastate to fbgl equivalents.
static CEventProcessor::keystate_t TranslateKeystate (utio::CKeyboard::metastate_t kbms)
{
    static const uint8_t metamap[] = {
	CEventProcessor::ks_Shift,
	CEventProcessor::ks_Alt,
	CEventProcessor::ks_Ctrl
    };
    CEventProcessor::keystate_t ks;
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

