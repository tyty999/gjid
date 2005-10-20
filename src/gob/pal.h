// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// pal.h
//

#ifndef PAL_H_1AAA82416DDBAF8A56F30C956E52D7CE
#define PAL_H_1AAA82416DDBAF8A56F30C956E52D7CE

#include <ustl.h>

namespace fbgl {
using namespace ustl;

typedef uint8_t		ray_t;
typedef uint8_t		color_t;
typedef uint32_t	colordef_t;

inline colordef_t RGB (ray_t r, ray_t g, ray_t b, ray_t a = 0)
    { return ((a << 24) | (r << 16) | (g << 8) | b); }
inline void unRGB (colordef_t c, ray_t& r, ray_t& g, ray_t& b)
    { b = c; c >>= 8; g = c; c >>= 8; r = c; }
inline void unRGBA (colordef_t c, ray_t& r, ray_t& g, ray_t& b, ray_t& a)
    { b = c; c >>= 8; g = c; c >>= 8; r = c; c >>= 8; a = c; }

typedef vector<colordef_t>	CPalette;

} // namespace fbgl

#endif

