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
				CGC (void);
    inline void			SetPixel (Point pt, color_t c)	{ at(pt) = c; }
    inline color_t		GetPixel (Point pt) const	{ return (at(pt)); }
    void			Clear (color_t c = 0);
    void			Bar (Rect r, color_t c);
    void			Box (Rect r, color_t c);
    void			HLine (Point pt, size_t l, color_t c);
    void			VLine (Point pt, size_t l, color_t c);
    void			Image (Rect r, const uint8_t* p);
    void			ImageMasked (Rect r, const uint8_t* p);
    void			GetImage (Rect r, uint8_t* p) const;
    void			Bitmap (Rect r, const uint8_t* p, color_t c);
};
