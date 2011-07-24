// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "config.h"

typedef uint8_t		ray_t;
typedef uint8_t		color_t;
typedef uint32_t	colordef_t;

inline colordef_t RGB (ray_t r, ray_t g, ray_t b, ray_t a = 0)
    { return ((a << 24) | (r << 16) | (g << 8) | b); }
inline void unRGB (colordef_t c, ray_t& r, ray_t& g, ray_t& b)
    { b = c; c >>= 8; g = c; c >>= 8; r = c; }
inline void unRGBA (colordef_t c, ray_t& r, ray_t& g, ray_t& b, ray_t& a)
    { b = c; c >>= 8; g = c; c >>= 8; r = c; c >>= 8; a = c; }
inline void setRayR (colordef_t& c, ray_t r)	{ c &= 0xFF00FFFF; c |= r << 16;}
inline void setRayG (colordef_t& c, ray_t g)	{ c &= 0xFFFF00FF; c |= g << 8;	}
inline void setRayB (colordef_t& c, ray_t b)	{ c &= 0xFFFFFF00; c |= b;	}
inline void setRayA (colordef_t& c, ray_t a)	{ c &= 0x00FFFFFF; c |= a << 24;}
inline ray_t getRayR (colordef_t c)		{ return (c >> 16); }
inline ray_t getRayG (colordef_t c)		{ return (c >> 8); }
inline ray_t getRayB (colordef_t c)		{ return (c); }
inline ray_t getRayA (colordef_t c)		{ return (c >> 24); }

/// \class CPalette pal.h fbgl.h
/// \brief Contains palette entries.
class CPalette : public vector<colordef_t> {
public:
    inline explicit	CPalette (size_t n = 0) : vector<colordef_t> (n) {}
    color_t		AllocColor (colordef_t c);
    inline color_t	AllocColor (ray_t r, ray_t g, ray_t b, ray_t a = 0)	{ return (AllocColor (RGB(r,g,b,a))); }
};
