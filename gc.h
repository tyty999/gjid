// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "config.h"

/// \class CGC gc.h fbgl.h
///
/// \brief Graphics context for drawing.
///
class CGC {
public:
    inline		CGC (void)		{ }
    inline uint32_t	AllocColor (int8_t r, int8_t g, int8_t b)	{ return (r<<16|g<<8|b); }
    inline void		SetPixel (int, int, uint8_t)	{ }
    inline int		Width (void) const	{ return (_width); }
    inline int		Height (void) const	{ return (_height); }
    inline void		Resize (int w, int h)	{ _width = w; _height = h; }
private:
    int			_width;
    int			_height;
};
