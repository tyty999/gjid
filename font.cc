// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "font.h"

CFont font;

CFont::CFont (void)
: m_Data (),
  m_LetterSize (0),
  m_Width (0),
  m_Height (0)
{
}

void CFont::read (istream& is)
{
    uint8_t w, h;
    is >> w >> h;
    Resize (w, h);
    is.read (m_Data.begin(), m_LetterSize * 256);
    is.align(4);
}

void CFont::write (ostream& os) const
{
    os << uint8_t(m_Width) << uint8_t(m_Height);
    os.write (m_Data.begin(), m_LetterSize * 256);
    os.align(4);
}

size_t CFont::stream_size (void) const
{
    return (Align (stream_size_of(uint8_t(m_Width)) +
		   stream_size_of(uint8_t(m_Height)) +
		   m_LetterSize * 256, 4));
}

memblock::iterator CFont::GetLetterStart (wchar_t c)
{
    return (m_Data.begin() + c * m_LetterSize);
}

void CFont::ActivatePoint (wchar_t c, coord_t xpos, coord_t ypos)
{
    memblock::iterator li = GetLetterStart (c);
    const uoff_t offset = ypos * m_Width + xpos;
    li [offset / 8] |= (1 << offset % 8);
}

void CFont::DeactivatePoint (wchar_t c, coord_t xpos, coord_t ypos)
{
    memblock::iterator li = GetLetterStart (c);
    const uoff_t offset = ypos * m_Width + xpos;
    li [offset / 8] &= (~(1 << offset % 8));
}

void CFont::TogglePoint (wchar_t c, coord_t xpos, coord_t ypos)
{
    memblock::iterator li = GetLetterStart (c);
    const uoff_t offset = ypos * m_Width + xpos;
    li [offset / 8] ^= (1 << offset % 8);
}

bool CFont::ReadPoint (wchar_t c, coord_t xpos, coord_t ypos)
{
    memblock::iterator li = GetLetterStart (c);
    const uoff_t offset = ypos * m_Width + xpos;
    return (li [offset / 8] & (1 << offset % 8));
}

void CFont::Resize (dim_t w, dim_t h)
{
    m_Width = w;
    m_Height = h;
    m_LetterSize = (m_Width * m_Height + 7) / 8;
    m_Data.resize (m_LetterSize * 256);
}

int CFont::PrintString (CGC& gc, coord_t x, coord_t y, const char* s, color_t color)
{
    size_t m = x;
    for (uoff_t n = 0; n < strlen(s); ++ n)
	m += PrintCharacter (gc, m, y, s[n], color);
    return (m - x);
}

int CFont::PrintCharacter (CGC& gc, coord_t x, coord_t y, wchar_t c, color_t color)
{
    memblock::const_iterator li = GetLetterStart (c);
    size_t bit = 0, cmw = 0;
    for (uoff_t j = 0; j < m_Height; ++ j) {
	for (uoff_t i = 0; i < m_Width; ++ i) {
	    if ((*li & (1 << bit)) > 0) {
		gc.SetPixel (Point(x + i, y + j), color);
		cmw = max (i, cmw);
	    }
	    if (bit < 7)
		++ bit;
	    else {
		bit = 0;
		++ li;
	    }
	}
    }
    return (cmw + 2);
}
