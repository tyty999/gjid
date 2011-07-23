// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "primi.h"

/// \class CImage image.h fbgl.h
///
/// \brief A rectangular pixel image object.
///
/// Supports 8-bit images of reasonable size. Reads and writes in GIF format.
///
class CImage {
public:
    enum EFlag {
	f_Transparent,
	f_SortedPalette,
	f_MergedPalette,
	f_Last
    };
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
			       ~CImage (void);
    void			Resize (const Size2d& sz);
    void			link (const memlink& l, const Size2d& sz);
    void			unlink (void);
    inline const_iterator	begin (void) const	{ return (m_Pixels.begin()); }
    inline iterator		begin (void)		{ return (m_Pixels.begin()); }
    inline const_iterator	end (void) const	{ return (m_Pixels.end()); }
    inline iterator		end (void)		{ return (m_Pixels.end()); }
    inline size_type		size (void) const	{ return (m_Pixels.size()); }
    inline size_type		capacity (void) const	{ return (m_Pixels.capacity()); }
    inline const CPalette&	Palette (void) const	{ return (m_Palette); }
    inline CPalette&		Palette (void)		{ return (m_Palette); }
    inline Size2d		Size (void) const	{ return (m_Size); }
    inline dim_t		Width (void) const	{ return (m_Size[0]); }
    inline dim_t		Height (void) const	{ return (m_Size[1]); }
    inline value_type&		at (Point p)		{ return (m_Pixels [p[1] * Width() + p[0]]); }
    inline value_type		at (Point p) const	{ return (m_Pixels [p[1] * Width() + p[0]]); }
    inline iterator		iat (Point p)		{ return (begin() + p[1] * Width() + p[0]); }
    inline const_iterator	iat (Point p) const	{ return (begin() + p[1] * Width() + p[0]); }
    inline bool			Flag (EFlag f) const		{ return (m_Flags[f]); }
    inline void			SetFlag (EFlag f, bool v =true)	{ m_Flags.set (f, v); }
    color_t			AllocColor (colordef_t c);
    inline color_t		AllocColor (ray_t r, ray_t g, ray_t b)	{ return (AllocColor (RGB(r,g,b))); }
    size_t			BitsPerPixel (void) const;
    void			MergePaletteInto (CPalette& pal);
    void			NormalizePalette (void);
    void			read (istream& is);
    void			write (ostream& os) const;
    size_t			stream_size (void) const;
private:
    void			ReadGifColormap (istream& is, size_t bpp);
    void			WriteGifColormap (ostream& os) const;
private:
    pixvec_t			m_Pixels;	///< Pixel data.
    CPalette			m_Palette;	///< Palette for the image.
    Size2d			m_Size;		///< Image dimensions.
    bitset<f_Last>		m_Flags;
};

ALIGNOF (CImage, 1)
STD_STREAMABLE (CImage)
