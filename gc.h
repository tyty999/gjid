// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "image.h"

/// \class CGC gc.h fbgl.h
///
/// \brief Graphics context for drawing.
///
class CGC : public CImage {
public:
    inline		CGC (void) : CImage() {}
    void		Clear (color_t c = 0);
    void		Box (Rect r, color_t c);
    inline void		SetPixel (Point pt, color_t c)	{ at(pt) = c; }
    void		Image (const CImage& img, coord_t x, coord_t y);
    void		ImageMasked (const CImage& img, coord_t x, coord_t y);
private:
    void		HLine (Point pt, size_t l, color_t c);
    void		VLine (Point pt, size_t l, color_t c);
};
