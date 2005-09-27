// fbdrv.cc
//

#include "fbdrv.h"

namespace fbgl {

CFramebuffer::CFramebuffer (void)
: m_Modes ()
{
}

/*static*/ CFramebuffer& CFramebuffer::Instance (void)
{
    static CFramebuffer s_fb;
    return (s_fb);
}

void CFramebuffer::Open (void)
{
}

void CFramebuffer::Close (void)
{
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

void CFramebuffer::SetMode (size_t w, size_t h, size_t, size_t freq)
{
    if (m_Modes.empty())
	LoadModes();
    uoff_t m = FindClosestMode (w, h, freq);
    if (m == m_Modes.size())
	return;
    cout.format ("Mode closest to %ux%u@%u:\n", w, h, freq);
    cout << m_Modes[m];
}

} // namespace fbgl

