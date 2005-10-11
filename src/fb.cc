// fb.cc
//

#include "fb.h"

namespace fbgl {

/// Default constructor.
CFramebuffer::CFramebuffer (void)
: m_Modes(),
  m_GC (),
  m_Buffer ()
{
}

/// Virtual destructor.
CFramebuffer::~CFramebuffer (void)
{
}

void CFramebuffer::SetStandardMode (EStdFbMode m, size_t freq)
{
    if (m == stdmode_320x240x8) {
	SetMode (FindClosestMode (640, 480, freq), 8);
	m_Buffer.resize (320 * 240);
	m_GC.link (m_Buffer, Size2d(320, 240));
    } else if (m == stdmode_640x480x8) {
	SetMode (FindClosestMode (640, 480, freq), 8);
    } else {
	m_GC.unlink();
	m_Buffer.resize (0);
    }
}

/// Sets mode \p m with \p depth.
void CFramebuffer::SetMode (CFbMode m, size_t depth)
{
    m_Buffer.resize (m.Width() * m.Height() * depth / 8);
    m_GC.link (m_Buffer, Size2d(m.Width(), m.Height()));
}

/// Performs actions on focus acquisition.
void CFramebuffer::OnFocus (bool bFocus)
{
    if (bFocus)
	Flush();
}

/// Loads available video modes from /etc/fb.modes
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

/// Looks up a video mode closest to the given parameters.
const CFbMode& CFramebuffer::FindClosestMode (size_t w, size_t h, size_t freq) const
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
    return (found < m_Modes.size() ? m_Modes[found] : CFbMode::null_Mode);
}

} // namespace fbgl

