// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
//
// gc.cc
//

#include "gc.h"

namespace fbgl {

/// Default constructor.
CGC::CGC (void)
: CImage ()
{
}

void CGC::Clear (color_t c)
{
    fill (begin(), end(), c);
}

void CGC::Bar (Rect r, color_t c)
{
    iterator i (iat (r[0]));
    for (uoff_t y = 0; y < r.Height(); ++y, i+=Width())
	fill_n (i, r.Width(), c);
}

void CGC::Box (Rect r, color_t c)
{
    HLine (r[0], r.Width(), c);
    VLine (r[0], r.Height(), c);
    HLine (Point(r[0][0],r[1][1]), r.Width()+1, c);
    VLine (Point(r[1][0],r[0][1]), r.Height(), c);
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

void CGC::Image (Rect r, const uint8_t* p)
{
    iterator i (iat (r[0]));
    for (uoff_t y = 0; y < r.Height(); ++y, i+=Width(), p+=r.Width())
	copy_n (p, r.Width(), i);
}

void CGC::ImageMasked (Rect r, const uint8_t* p)
{
    iterator i (iat (r[0]));
    for (uoff_t y = 0; y < r.Height(); ++y, i+=(Width()-r.Width()))
	for (uoff_t x = 0; x < r.Width(); ++x, ++i, ++p)
	    if (*p)
		*i = *p;
}

void CGC::GetImage (Rect r, uint8_t* p) const
{
    const_iterator i (iat (r[0]));
    for (uoff_t y = 0; y < r.Height(); ++y, i+=Width(), p+=r.Width())
	copy_n (i, r.Width(), p);
}

void CGC::Bitmap (Rect r, const uint8_t* p, color_t c)
{
    iterator i (iat (r[0]));
    uoff_t bit = 0;
    for (uoff_t y = 0; y < r.Height(); ++y, i+=(Width()-r.Width())) {
	for (uoff_t x = 0; x < r.Width(); ++x, ++i) {
	    if (*p & (1 << bit))
		*i = c;
	    p += !(bit = (bit + 1) % 8);
	}
    }
}

} // namespace fbgl

