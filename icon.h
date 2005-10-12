// icon.h
//

#ifndef ICON_H_2BA53522369CAE38636AFA57387DDC64
#define ICON_H_2BA53522369CAE38636AFA57387DDC64

#include <fbgl.h>
using namespace fbgl;

#define ICON_ID_STRING			"ICON"
#define ICON_ID_STRING_LENGTH		4		       

/// \class Icon icon.h "icon.h"
///
/// \brief A simple image object.
///
class Icon {
public:
				Icon (dim_t w = 1, dim_t h = 1, const color_t* p = NULL);
    void			SetImage (dim_t Width, dim_t Height, const color_t* p = NULL);
    inline Rect			Area (coord_t x, coord_t y) const		{ return (Rect (x, y, x + Width(), y + Height())); }
    inline void			Put (CGC& gc, coord_t x, coord_t y) const	{ gc.Image (Area(x,y), m_Bits.begin()); }
    inline void			PutMasked (CGC& gc, coord_t x, coord_t y) const	{ gc.ImageMasked (Area(x,y), m_Bits.begin()); }
    inline void			Get (CGC& gc, coord_t x, coord_t y)		{ gc.GetImage (Area(x,y), m_Bits.begin()); }
    inline dim_t		Width (void) const				{ return (m_Width); }
    inline dim_t		Height (void) const				{ return (m_Height); }
    inline const color_t*	GetRow (coord_t row) const			{ return (m_Bits.begin() + row * Width()); }
    inline color_t*		GetRow (coord_t row)				{ return (m_Bits.begin() + row * Width()); }
    inline void			SetPixel (coord_t x, coord_t y, color_t c)	{ GetRow(y)[x] = c; }
    inline color_t		GetPixel (coord_t x, coord_t y) const		{ return (GetRow (y)[x]); }
    void			read (istream& is);
    void			write (ostream& os) const;
    size_t			stream_size (void) const;
protected:
    vector<color_t>		m_Bits;
    dim_t			m_Width;
    dim_t			m_Height;
};	

//----------------------------------------------------------------------

STD_STREAMABLE (Icon)

//----------------------------------------------------------------------

#endif

