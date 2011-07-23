// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "gc.h"

void CGC::Clear (color_t c)
{
    fill (begin(), end(), c);
}

void CGC::HLine (Point pt, size_t l, color_t c)
{
    fill_n (iat(pt), l, c);
}

void CGC::VLine (Point pt, size_t l, color_t c)
{
    for (iterator i (iat (pt)); l--; i += Width())
	*i = c;
}

void CGC::Box (Rect r, color_t c)
{
    HLine (r[0], r.Width(), c);
    VLine (r[0], r.Height(), c);
    HLine (Point(r[0][0],r[1][1]), r.Width()+1, c);
    VLine (Point(r[1][0],r[0][1]), r.Height(), c);
}

void CGC::Image (const CImage& img, coord_t x, coord_t y)
{
    iterator d (iat (Point (x,y)));
    const uint8_t* s = img.begin();
    for (unsigned l = 0; l < img.Height(); ++l, d+=Width(), s+=img.Width())
	copy_n (s, img.Width(), d);
}

void CGC::ImageMasked (const CImage& img, coord_t x, coord_t y)
{
    iterator d (iat (Point (x,y)));
    const uint8_t* s = img.begin();
    for (unsigned l = 0; l < img.Height(); ++l, d+=Width()-img.Width())
	for (unsigned c = 0; c < img.Width(); ++c, ++s, ++d)
	    if (*s)
		*d = *s;
}
