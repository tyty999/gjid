// gc.h
//

#ifndef GC_H_355C59EC4607EE327E5914582DC9A02E
#define GC_H_355C59EC4607EE327E5914582DC9A02E

#include "primi.h"

namespace fbgl {

/// \class CGC gc.h fbgl.h
///
/// \brief Graphics context for drawing.
///
class CGC {
public:
    typedef uint8_t		pixel_t;
    typedef pixel_t		value_type;
    typedef value_type*		pointer;
    typedef const value_type*	const_pointer;
    typedef pointer		iterator;
    typedef const_pointer	const_iterator;
    typedef memlink::size_type	size_type;
    typedef ptrdiff_t		difference_type;
public:
				CGC (void);
    inline iterator		begin (void)		{ return (iterator (m_Pix.begin())); };
    inline const_iterator	begin (void) const	{ return (const_iterator (m_Pix.begin())); };
    inline size_type		size (void) const	{ return (m_Pix.size()); }
    inline iterator		end (void)		{ return (begin() + size()); }
    inline const_iterator	end (void) const	{ return (begin() + size()); }
    inline iterator		at (Point pt)		{ return (begin() + pt[1] * m_Size[1] + pt[0]); }
    inline const_iterator	at (Point pt) const	{ return (begin() + pt[1] * m_Size[1] + pt[0]); }
    inline const Size2d&	Size (void) const	{ return (m_Size); }
    void			link (memlink l, Size2d sz);
    void			unlink (void);
    inline CPalette&		Palette (void)		{ return (m_Pal); }
    inline const CPalette&	Palette (void) const	{ return (m_Pal); }
    void			SetPixel (Point pt, color_t c);
    color_t			GetPixel (Point pt) const;
    void			Bar (Rect r, color_t c);
    void			Box (Rect r, color_t c);
    void			HLine (Point pt, size_t l, color_t c);
    void			VLine (Point pt, size_t l, color_t c);
    void			Image (Rect r, const uint8_t* p);
    void			Bitmap (Rect r, const uint8_t* p, color_t c);
private:
    memlink			m_Pix;	///< Pixel data.
    Size2d			m_Size;	///< Dimensions of the pixel data.
    CPalette			m_Pal;	///< Color palette.
};

} // namespace fbgl

#endif

