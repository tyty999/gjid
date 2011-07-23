// This file is part of the GJID game.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// font.h
//

#ifndef FONT_H_39BD3A88149536EF6573B7D448F4649E
#define FONT_H_39BD3A88149536EF6573B7D448F4649E

#include "fbgl.h"
using namespace fbgl;

class Font {
public:
			Font (void);
    int			PrintCharacter (CGC& gc, coord_t x, coord_t y, wchar_t c, color_t color);
    int			PrintString (CGC& gc, coord_t x, coord_t y, const char* s, color_t color);
    void		ActivatePoint (wchar_t c, coord_t xpos, coord_t ypos);
    void		DeactivatePoint (wchar_t c, coord_t xpos, coord_t ypos);
    void		TogglePoint (wchar_t c, coord_t xpos, coord_t ypos);
    bool		ReadPoint (wchar_t c, coord_t xpos, coord_t ypos);
    void		Resize (dim_t w, dim_t h);
    inline uint8_t	Width (void)	{ return (m_Width); }
    inline uint8_t	Height (void)	{ return (m_Height); }
    void		read (istream& is);
    void		write (ostream& os) const;
    size_t		stream_size (void) const;
protected:
    memblock::iterator	GetLetterStart (wchar_t c);
private:
    memblock		m_Data;
    size_t		m_LetterSize;
    dim_t		m_Width;
    dim_t		m_Height;
};

extern Font font;

STD_STREAMABLE (Font)

#endif

