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
class Icon : public CImage {
public:
				Icon (dim_t w = 1, dim_t h = 1, const color_t* p = NULL);
    void			SetImage (dim_t Width, dim_t Height, const color_t* p = NULL);
    inline Rect			Area (coord_t x, coord_t y) const		{ return (Rect (x, y, x + Width(), y + Height())); }
    inline void			Put (CGC& gc, coord_t x, coord_t y) const	{ gc.Image (Area(x,y), begin()); }
    inline void			PutMasked (CGC& gc, coord_t x, coord_t y) const	{ gc.ImageMasked (Area(x,y), begin()); }
    inline void			Get (CGC& gc, coord_t x, coord_t y)		{ gc.GetImage (Area(x,y), begin()); }
    inline void			SetPixel (coord_t x, coord_t y, color_t c)	{ at(Point(x,y)) = c; }
    inline color_t		GetPixel (coord_t x, coord_t y) const		{ return (at(Point(x,y))); }
    void			read (istream& is);
    void			write (ostream& os) const;
    size_t			stream_size (void) const;
};	

//----------------------------------------------------------------------

STD_STREAMABLE (Icon)

//----------------------------------------------------------------------

#endif

