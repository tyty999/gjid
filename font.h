// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "gc.h"

typedef uint8_t color_t;

class CFont {
public:
			CFont (void);
    int			PrintString (CGC& gc, int x, int y, const char* s, color_t color);
    void		read (istream& is);
private:
    int			PrintCharacter (CGC& gc, int x, int y, wchar_t c, color_t color);
    memblock::iterator	GetLetterStart (wchar_t c)	{ return (_data.begin() + c * _letterSize); }
private:
    memblock		_data;
    size_t		_letterSize;
    uint16_t		_width;
    uint16_t		_height;
};

STD_STREAMABLE (CFont)
