// xfb.cc
//

#include "xfb.h"

namespace fbgl {

CXlibFramebuffer::CXlibFramebuffer (void)
: CFramebuffer (),
  m_Modes ()
{
}

CXlibFramebuffer::~CXlibFramebuffer (void)
{
}

/*static*/ CXlibFramebuffer& CXlibFramebuffer::Instance (void)
{
    static CXlibFramebuffer s_Fb;
    return (s_Fb);
}

void CXlibFramebuffer::Open (void)
{
}

void CXlibFramebuffer::Close (void)
{
}

void CXlibFramebuffer::SetMode (CFbMode, size_t)
{
}

void CXlibFramebuffer::OnFocus (bool)
{
}

void CXlibFramebuffer::CheckEvents (CEventProcessor*) const
{
}

memlink CXlibFramebuffer::Pixels (void)
{
    return (memlink());
}

Size2d CXlibFramebuffer::Size (void)
{
    return (Size2d());
}

void CXlibFramebuffer::Flush (const CGC&)
{
}

/// Loads available video modes from /etc/fb.modes
void CXlibFramebuffer::LoadModes (void)
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
const CFbMode& CXlibFramebuffer::FindClosestMode (size_t w, size_t h, size_t freq) const
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

