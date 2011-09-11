// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "font.h"

CFont::CFont (void)
:_data()
,_letterSize (0)
,_width (0)
,_height (0)
{
}

void CFont::read (istream& is)
{
    uint8_t w, h;
    is >> w >> h;
    _width = w;
    _height = h;
    _letterSize = (_width * _height + 7) / 8;
    _data.resize (_letterSize * 256);
    is.read (_data.begin(), _letterSize * 256);
    is.align(4);
}


int CFont::PrintString (CGC& gc, int x, int y, const char* s, color_t color)
{
    size_t m = x;
    for (uoff_t n = 0; n < strlen(s); ++ n)
	m += PrintCharacter (gc, m, y, s[n], color);
    return (m - x);
}

int CFont::PrintCharacter (CGC& gc, int x, int y, wchar_t c, color_t color)
{
    memblock::const_iterator li = GetLetterStart (c);
    size_t bit = 0, cmw = 0;
    for (uoff_t j = 0; j < _height; ++ j) {
	for (uoff_t i = 0; i < _width; ++ i) {
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
