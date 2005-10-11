// icon.h
//

#ifndef ICON_H_2BA53522369CAE38636AFA57387DDC64
#define ICON_H_2BA53522369CAE38636AFA57387DDC64

#include <fbgl.h>
using namespace fbgl;

#define ICON_ID_STRING			"ICON"
#define ICON_ID_STRING_LENGTH		4		       

typedef enum {
    NoBlend,
    SeeThroughBlend,
    AndBlend,
    XorBlend,
    OrBlend,
    LastBlend
} BlendType;

class Icon {
public:
				Icon (dim_t w = 1, dim_t h = 1, const color_t* p = NULL);
    void			Put (CGC& gc, coord_t x, coord_t y) const;
    void			Get (CGC& gc, coord_t x, coord_t y);
    inline dim_t		Width (void) const	{ return (m_Width); }
    inline dim_t		Height (void) const	{ return (m_Height); }
    void			SetImage (dim_t Width, dim_t Height, const color_t* p = NULL);
    void			SetPixel (coord_t x, coord_t y, color_t color);
    color_t			GetPixel (coord_t x, coord_t y) const;
    void			SetRow (coord_t row, const color_t* p);
    void			SetCol (coord_t col, const color_t* p);
    inline const color_t*	GetRow (coord_t row) const	{ return (m_Bits.begin() + row * Width()); }
    inline color_t*		GetRow (coord_t row)		{ return (m_Bits.begin() + row * Width()); }

    void			BlendWith (const Icon& src, BlendType how);
    void			read (istream& is);
    void			write (ostream& os) const;
    size_t			stream_size (void) const;
protected:
    vector<color_t>		m_Bits;
    dim_t			m_Width;
    dim_t			m_Height;
};	

STD_STREAMABLE (Icon)

#endif

