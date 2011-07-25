// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "font.h"

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
    m_Width = w;
    m_Height = h;
    m_LetterSize = (m_Width * m_Height + 7) / 8;
    m_Data.resize (m_LetterSize * 256);
    is.read (m_Data.begin(), m_LetterSize * 256);
    is.align(4);
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
		gc.SetPixel (x+i, y+j, color);
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
