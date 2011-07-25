// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "gc.h"

void CGC::Clear (color_t c)
{
    fill (begin(), end(), c);
}

void CGC::Image (const CImage& img, coord_t x, coord_t y)
{
    iterator d (iat (x,y));
    const uint8_t* s = img.begin();
    for (unsigned l = 0; l < img.Height(); ++l, d+=Width(), s+=img.Width())
	copy_n (s, img.Width(), d);
}

void CGC::ImageMasked (const CImage& img, coord_t x, coord_t y)
{
    iterator d (iat (x,y));
    const uint8_t* s = img.begin();
    for (unsigned l = 0; l < img.Height(); ++l, d+=Width()-img.Width())
	for (unsigned c = 0; c < img.Width(); ++c, ++s, ++d)
	    if (*s)
		*d = *s;
}
