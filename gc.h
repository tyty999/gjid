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
    inline void		SetPixel (coord_t x, coord_t y, color_t c)	{ at(x,y) = c; }
    void		Image (const CImage& img, coord_t x, coord_t y);
    void		ImageMasked (const CImage& img, coord_t x, coord_t y);
};
