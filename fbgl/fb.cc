// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "fb.h"

/// Default constructor.
CFramebuffer::CFramebuffer (void)
: m_Modes(),
  m_OrigMode (),
  m_CurMode (),
  m_GC ()
{
}

/// Virtual destructor.
CFramebuffer::~CFramebuffer (void)
{
}

void CFramebuffer::Open (void)
{
    LoadModes (m_Modes);
}

void CFramebuffer::SetStandardMode (EStdFbMode m, size_t freq)
{
    if (m)
	SetMode (FindClosestMode (640, 480, freq), 8);
    m_GC.Resize (Size2d (m * 320, m * 240));
}

/// Sets mode \p m with \p depth.
void CFramebuffer::SetMode (rcmode_t m, size_t)
{
    m_GC.Resize (Size2d (m.Width(), m.Height()));
}

/// Performs actions on focus acquisition.
void CFramebuffer::OnFocus (bool bFocus)
{
    if (bFocus)
	Flush();
}

/// Looks up a video mode closest to the given parameters.
const CXlibMode& CFramebuffer::FindClosestMode (size_t w, size_t h, size_t freq) const
{
    uoff_t found (m_Modes.size());
    size_t diff (SIZE_MAX);
    for (uoff_t i = 0; i < m_Modes.size(); ++i) {
	const CXlibMode& m (m_Modes[i]);
	const size_t md = absv<int>(m.Width() - w) +
			  absv<int>(m.Height() - h) +
			  absv<int>(m.RefreshRate() - freq);
	if (md < diff)
	    found = i;
	diff = min (diff, md);
    }
    const CXlibMode& foundNode (m_Modes[found]);
    return (found < m_Modes.size() ? foundNode : CXlibMode::null_Mode);
}
