// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "gc.h"

class CFont {
public:
			CFont (void);
    int			PrintString (CGC& gc, coord_t x, coord_t y, const char* s, color_t color);
    void		read (istream& is);
private:
    int			PrintCharacter (CGC& gc, coord_t x, coord_t y, wchar_t c, color_t color);
    memblock::iterator	GetLetterStart (wchar_t c)	{ return (_data.begin() + c * _letterSize); }
private:
    memblock		_data;
    size_t		_letterSize;
    dim_t		_width;
    dim_t		_height;
};

STD_STREAMABLE (CFont)
