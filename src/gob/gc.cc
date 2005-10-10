// gc.cc
//

#include "gc.h"

namespace fbgl {

/// Default constructor.
CGC::CGC (void)
: m_Pix (),
  m_Size ()
{
}

/// Attaches to pixels at \p l of size \p sz.
void CGC::link (memlink l, Size2d sz)
{
    m_Pix.link (l);
    m_Size = sz;
}

/// Detaches from the current pixel data.
void CGC::unlink (void)
{
    m_Pix.unlink();
    m_Size = Size2d();
}

void CGC::SetPixel (Point pt, color_t c)
{
    *at(pt) = c;
}

color_t CGC::GetPixel (Point pt) const
{
    return (*at(pt));
}

void CGC::Bar (Rect r, color_t c)
{
    iterator i (at (r[0]));
    for (uoff_t y = 0; y < r.Height(); ++y, i+=m_Size[0])
	fill_n (i, r.Width(), c);
}

void CGC::Box (Rect r, color_t c)
{
    HLine (r[0], r.Width(), c);
    VLine (r[1], r.Height(), c);
    HLine (Point(r[0][0],r[1][1]), r.Width(), c);
    VLine (Point(r[1][0],r[0][1]), r.Height(), c);
}

void CGC::HLine (Point pt, size_t l, color_t c)
{
    fill_n (at(pt), l, c);
}

void CGC::VLine (Point pt, size_t l, color_t c)
{
    for (iterator i (at (pt)); l--; i += m_Size[0])
	*i = c;
}

void CGC::Image (Rect r, const uint8_t* p)
{
    iterator i (at (r[0]));
    for (uoff_t y = 0; y < r.Height(); ++y, i+=m_Size[0], p+=r.Width())
	copy_n (p, r.Width(), i);
}

void CGC::ImageMasked (Rect r, const uint8_t* p)
{
    iterator i (at (r[0]));
    for (uoff_t y = 0; y < r.Height(); ++y, i+=(m_Size[0]-r.Width()))
	for (uoff_t x = 0; x < r.Width(); ++x, ++i, ++p)
	    if (*p)
		*i = *p;
}

void CGC::GetImage (Rect r, uint8_t* p) const
{
    const_iterator i (at (r[0]));
    for (uoff_t y = 0; y < r.Height(); ++y, i+=m_Size[0], p+=r.Width())
	copy_n (i, r.Width(), p);
}

void CGC::Bitmap (Rect r, const uint8_t* p, color_t c)
{
    iterator i (at (r[0]));
    uoff_t bit = 0;
    for (uoff_t y = 0; y < r.Height(); ++y, i+=(m_Size[0]-r.Width())) {
	for (uoff_t x = 0; x < r.Width(); ++x, ++i) {
	    if (*p & (1 << bit))
		*i = c;
	    p += !(bit = (bit + 1) % 8);
	}
    }
}

} // namespace fbgl

