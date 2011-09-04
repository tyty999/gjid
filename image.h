// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "pal.h"

/// \class CImage image.h fbgl.h
///
/// \brief A rectangular pixel image object.
///
/// Supports 8-bit images of reasonable size. Reads and writes in GIF format.
///
class CImage {
public:
    typedef color_t			pixel_t;
    typedef pixel_t			value_type;
    typedef vector<value_type>		pixvec_t;
    typedef pixvec_t::pointer		pointer;
    typedef pixvec_t::const_pointer	const_pointer;
    typedef pixvec_t::iterator		iterator;
    typedef pixvec_t::const_iterator	const_iterator;
    typedef pixvec_t::size_type		size_type;
    typedef pixvec_t::difference_type	difference_type;
public:
				CImage (void);
				CImage (dim_t w, dim_t h, const color_t* p = NULL);
    void			Resize (dim_t w, dim_t h);
    inline const_iterator	begin (void) const			{ return (_pixels.begin()); }
    inline iterator		begin (void)				{ return (_pixels.begin()); }
    inline const_iterator	end (void) const			{ return (_pixels.end()); }
    inline iterator		end (void)				{ return (_pixels.end()); }
    inline const CPalette&	Palette (void) const			{ return (_palette); }
    inline CPalette&		Palette (void)				{ return (_palette); }
    inline dim_t		Width (void) const			{ return (_width); }
    inline dim_t		Height (void) const			{ return (_height); }
    inline value_type&		at (coord_t x, coord_t y)		{ return (_pixels [y * Width() + x]); }
    inline value_type		at (coord_t x, coord_t y) const		{ return (_pixels [y * Width() + x]); }
    inline iterator		iat (coord_t x, coord_t y)		{ return (begin() + y * Width() + x); }
    inline const_iterator	iat (coord_t x, coord_t y) const	{ return (begin() + y * Width() + x); }
    color_t			AllocColor (colordef_t c)		{ return (_palette.AllocColor (c)); }
    inline color_t		AllocColor (ray_t r, ray_t g, ray_t b)	{ return (AllocColor (RGB(r,g,b))); }
    void			MergePaletteInto (CPalette& pal);
    void			read (istream& is);
private:
    void			ReadGifColormap (istream& is, size_t bpp);
private:
    pixvec_t			_pixels;	///< Pixel data.
    CPalette			_palette;	///< Palette for the image.
    dim_t			_width;
    dim_t			_height;
};

ALIGNOF (CImage, 1)
STD_STREAMABLE (CImage)
