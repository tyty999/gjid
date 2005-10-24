// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// fb.cc
//

#include "fb.h"

namespace fbgl {

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
    if (m == stdmode_320x240x8) {
	SetMode (FindClosestMode (640, 480, freq), 8);
	m_GC.Resize (Size2d (320, 240));
    } else if (m == stdmode_640x480x8)
	SetMode (FindClosestMode (640, 480, freq), 8);
    else
	m_GC.Resize (Size2d());
}

/// Sets mode \p m with \p depth.
void CFramebuffer::SetMode (CMode m, size_t)
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
const CMode& CFramebuffer::FindClosestMode (size_t w, size_t h, size_t freq) const
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
    return (found < m_Modes.size() ? m_Modes[found] : CMode::null_Mode);
}

} // namespace fbgl

