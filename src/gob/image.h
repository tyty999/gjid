/// This file is part of the fbgl library.
///
/// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
/// This file is free software, distributed under the MIT License.
/// 
/// \file image.h
///

#ifndef IMAGE_H_48CDC049577DEEC84CA92EA51BA0E7C0
#define IMAGE_H_48CDC049577DEEC84CA92EA51BA0E7C0

#include "primi.h"

namespace fbgl {

/// \class CImage image.h fbgl.h
///
/// \brief A rectangular pixel image object.
///
/// Supports 8-bit images of reasonable size. Reads and writes in GIF format.
///
class CImage {
public:
    typedef color_t			value_type;
    typedef vector<value_type>		pixvec_t;
    typedef pixvec_t::iterator		iterator;
    typedef pixvec_t::const_iterator	const_iterator;
    typedef pixvec_t::size_type		size_type;
    typedef pixvec_t::difference_type	difference_type;
public:
				CImage (void);
    void			Resize (dim_t w, dim_t h);
    inline const_iterator	begin (void) const	{ return (m_Pixels.begin()); }
    inline iterator		begin (void)		{ return (m_Pixels.begin()); }
    inline const_iterator	end (void) const	{ return (m_Pixels.end()); }
    inline iterator		end (void)		{ return (m_Pixels.end()); }
    inline const CPalette&	Palette (void) const	{ return (m_Palette); }
    inline CPalette&		Palette (void)		{ return (m_Palette); }
    inline dim_t		Width (void) const	{ return (m_Size[0]); }
    inline dim_t		Height (void) const	{ return (m_Size[1]); }
    inline value_type&		at (coord_t x, coord_t y)	{ return (m_Pixels [y * Width() + x]); }
    inline value_type		at (coord_t x, coord_t y) const	{ return (m_Pixels [y * Width() + x]); }
    size_t			BitsPerPixel (void) const;
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
};

} // namespace fbgl

STD_STREAMABLE (fbgl::CImage)

#endif

