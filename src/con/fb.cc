// fbdrv.cc
//

#include "fbdrv.h"
#include <linux/fb.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <sys/stat.h>
#include <unistd.h>

namespace fbgl {

CFramebuffer::CFramebuffer (void)
: m_pFix (NULL),
  m_pOrigVar (NULL),
  m_pVar (NULL),
  m_Device (),
  m_Modes ()
{
    auto_ptr<fb_fix_screeninfo> pFix (new fb_fix_screeninfo);
    auto_ptr<fb_var_screeninfo> pOrigVar (new fb_var_screeninfo);
    auto_ptr<fb_var_screeninfo> pVar (new fb_var_screeninfo);
    m_pFix = pFix.release();
    m_pOrigVar = pOrigVar.release();
    m_pVar = pVar.release();
}

CFramebuffer::~CFramebuffer (void)
{
    Close();
    Delete (m_pFix);
    Delete (m_pOrigVar);
    Delete (m_pVar);
}

/*static*/ CFramebuffer& CFramebuffer::Instance (void)
{
    static CFramebuffer s_fb;
    return (s_fb);
}

void CFramebuffer::Open (void)
{
    assert (!m_Device.IsOpen());
    string deviceName;
    DetectDefaultDevice (deviceName);
    m_Device.Open (deviceName);
    m_Device.Ioctl (IOCTLID (FBIOGET_FSCREENINFO), m_pFix);
    m_Device.Ioctl (IOCTLID (FBIOGET_VSCREENINFO), m_pOrigVar);
    *m_pVar = *m_pOrigVar;
    m_Screen = m_Device.Map (m_pFix->smem_len);
}

void CFramebuffer::Close (void)
{
    if (m_Screen.data())
	m_Device.Unmap (m_Screen);
    if (m_Device.IsOpen()) {
	m_Device.Ioctl (IOCTLID (FBIOPUT_VSCREENINFO), m_pOrigVar);
	m_Device.Close();
    }
}

/// Detects the default framebuffer device for use with the attached tty.
///
/// This works by trying to open fb0 and getting the console-framebuffer
/// mapping for the standard input device.
///
void CFramebuffer::DetectDefaultDevice (string& deviceName) const
{
    struct stat ttyStat;
    if (fstat (STDIN_FILENO, &ttyStat))
	throw file_exception ("fstat", "stdin");
    const int vti = ((MAJOR(ttyStat.st_rdev) == TTY_MAJOR) ? int(MINOR(ttyStat.st_rdev)) : -1);
    if (vti < 0)
	throw domain_error ("this program only works on a real console, not in an xterm or ssh");
    CFile fb;
    fb.Open ("/dev/fb0", CFile::mode_Write);
    struct fb_con2fbmap c2fb = { vti, 0 };
    fb.Ioctl (IOCTLID (FBIOGET_CON2FBMAP), &c2fb);
    deviceName.format ("/dev/fb%d", c2fb.framebuffer);
}

void CFramebuffer::LoadModes (void)
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

uoff_t CFramebuffer::FindClosestMode (size_t w, size_t h, size_t freq) const
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
    return (found);
}

void CFramebuffer::SetMode (size_t w, size_t h, size_t freq, size_t depth)
{
    assert (m_Device.IsOpen());
    if (m_Modes.empty())
	LoadModes();
    uoff_t m = FindClosestMode (w, h, freq);
    if (m == m_Modes.size())
	return;
    CFbMode newMode (m_Modes[m]);
    newMode.SetDepth (depth);
    newMode.CreateVarinfo (*m_pVar);
    m_Device.Ioctl (IOCTLID (FBIOPUT_VSCREENINFO), m_pVar);
    cout.format ("Mode closest to %ux%u@%u:\n", w, h, freq);
    cout << m_Modes[m];
}

} // namespace fbgl

